package org.strykeforce.deadeye.cli

import com.codahale.metrics.Meter
import com.codahale.metrics.MetricRegistry
import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.arguments.multiple
import com.squareup.moshi.Moshi
import org.fusesource.jansi.Ansi.Attribute.INTENSITY_FAINT
import org.fusesource.jansi.Ansi.ansi
import org.fusesource.jansi.AnsiConsole
import org.jline.terminal.TerminalBuilder
import org.jline.utils.InfoCmp
import org.strykeforce.deadeye.*
import java.net.Socket
import java.net.URL
import kotlin.concurrent.timer

private val metrics = MetricRegistry()

class Watch : CliktCommand() {

  private val ids by argument().multiple()

  override fun run() {
    AnsiConsole.systemInstall()
    val prevLink = Deadeye.link
    Deadeye.link = Deadeye.getCamera<CenterTargetData>(ids.first()).stream.toLink()
    println()
    println(ansi().fgBrightBlue().a("id  serial  tgt     x     y   fps  drop"))
    println(ansi().a("=========================================").reset())

    val watchers = ids.map { Deadeye.getCamera<CenterTargetData>(it) }.map(::Watcher)
    repeat(watchers.size + 1) { println() }

    val timer = timer(period = 250) {
      print(ansi().cursorUpLine(watchers.size + 1))
      watchers.forEach(Watcher::output)
      println()
    }

    val terminal = TerminalBuilder.builder().jansi(true).system(true).build()
    terminal.puts(InfoCmp.Capability.cursor_invisible)
    terminal.enterRawMode()
    terminal.reader().read()

    watchers.forEach { it.camera.enabled = false }
    timer.cancel()

    Deadeye.link = prevLink
    terminal.close()
    AnsiConsole.systemUninstall()
  }
}

class Watcher(val camera: Camera<CenterTargetData>) : TargetDataListener {

  init {
    camera.targetDataListener = this
    camera.enabled = true
    camera.jsonAdapter = CenterTargetDataJsonAdapter(Moshi.Builder().build())
  }

  private var td = CenterTargetData()
  private var dropped = 0
  private val fpsMeter: Meter by lazy { metrics.meter(camera.id) }


  override fun onTargetData(data: TargetData) {
    if (data.sn - td.sn > 1) dropped += data.sn - td.sn
    td = data as CenterTargetData
    fpsMeter.mark()
  }

  fun output() {
    val id = ansi().fgBrightBlue().a(td.id).reset()
    val sn = ansi().a(INTENSITY_FAINT).a(td.sn.toString().padStart(6)).reset()
    val valid = if (td.valid) ansi().fgBrightGreen().a("Y").reset() else ansi().fgBrightRed().a("N").reset()
    val x = td.x.toInt().toString().padStart(5)
    val y = td.y.toInt().toString().padStart(5)
//    val x = 0.toString().padStart(5)
//    val y = 0.toString().padStart(5)
    val fps = ansi().fgBrightYellow().a("%5.1f".format(fpsMeter.oneMinuteRate)).reset()
    val d =
      ansi().let { if (dropped == 0) it.a(INTENSITY_FAINT) else it.fgBrightRed() }.a(dropped.toString().padStart(4))
        .reset()
    println("$id  $sn   $valid  $x $y $fps  $d")
  }
}

fun Camera.Stream.toLink(): Deadeye.Link = Socket(URL(this.url).host, 22).use {
  Deadeye.Link(it.localAddress.hostAddress, 5800, true)
}
