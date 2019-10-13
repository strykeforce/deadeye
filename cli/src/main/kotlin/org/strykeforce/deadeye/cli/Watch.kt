package org.strykeforce.deadeye.cli

import com.codahale.metrics.Meter
import com.codahale.metrics.MetricRegistry
import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.arguments.multiple
import org.fusesource.jansi.Ansi.ansi
import org.fusesource.jansi.AnsiConsole
import org.jline.terminal.TerminalBuilder
import org.strykeforce.deadeye.Camera
import org.strykeforce.deadeye.Deadeye
import org.strykeforce.deadeye.TargetData
import org.strykeforce.deadeye.TargetDataListener
import java.net.Socket
import java.net.URL
import kotlin.concurrent.timer

private const val LINES = 5

private val metrics = MetricRegistry()

class Watch : CliktCommand() {

  private val ids by argument().multiple()

  override fun run() {
    AnsiConsole.systemInstall()
    val prevLink = Deadeye.link
    Deadeye.link = Deadeye.getCamera(ids.first()).stream.toLink()
    println("ID  serial  valid     X     Y   FPS  drop")
    println("=========================================")

    val watchers = ids.map(Deadeye::getCamera).map(::Watcher)
    watchers.forEach { _ -> println() }

    val timer = timer(period = 250) {
      print(ansi().cursorUpLine(watchers.size))
      watchers.forEach(Watcher::output)
    }

    val terminal = TerminalBuilder.builder().jansi(true).system(true).build()
    terminal.enterRawMode()
    terminal.reader().read()

    watchers.forEach { it.camera.enabled = false }
    timer.cancel()

    Deadeye.link = prevLink
    terminal.close()
    AnsiConsole.systemUninstall()
  }
}

class Watcher(val camera: Camera) : TargetDataListener {

  init {
    camera.targetDataListener = this
    camera.enabled = true
  }

  private var td = TargetData(camera.id)
  private var dropped = 0
  private val fpsMeter: Meter by lazy { metrics.meter(camera.id) }

  override fun onTargetData(data: TargetData) {
    if (data.sn - td.sn > 1) dropped += data.sn - td.sn
    td = data
    fpsMeter.mark()
  }

  fun output() {
    val sn = td.sn.toString().padStart(6)
    val valid = td.valid.toString().padStart(6)
    val x = td.x.toInt().toString().padStart(5)
    val y = td.y.toInt().toString().padStart(5)
    val fps = "%5.1f".format(fpsMeter.oneMinuteRate)
    val d = dropped.toString().padStart(4)
    println(ansi().a("${td.id}  $sn $valid $x $y $fps  $d"))
  }
}

fun Camera.Stream.toLink(): Deadeye.Link = Socket(URL(this.url).host, 22).use {
  Deadeye.Link(it.localAddress.hostAddress, 5800, true)
}
