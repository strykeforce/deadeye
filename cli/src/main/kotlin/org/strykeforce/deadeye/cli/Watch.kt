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
import java.text.MessageFormat

private const val LINES = 5

class Watch : CliktCommand(), TargetDataListener {

  private val id by argument().multiple()
  private val metrics = MetricRegistry()
  private val fps: Meter by lazy { metrics.meter("messages") }

  override fun onTargetData(data: TargetData) {
    fps.mark()
    println(ansi().cursorUpLine(LINES).cursorToColumn(0).a("Serial: ${data.sn}"))
    println(ansi().cursorToColumn(0).a(" Valid: ${data.valid}"))
    println(ansi().cursorToColumn(0).a("     X: ${data.x.toInt()}"))
    println(ansi().cursorToColumn(0).a("     Y: ${data.y.toInt()}"))
    println(ansi().cursorToColumn(0).a("   FPS: ${fps.oneMinuteRate.toInt()}"))
  }

  private fun linkFromStream(stream: Camera.Stream): Deadeye.Link = Socket(URL(stream.url).host, 22).use {
    Deadeye.Link(it.localAddress.hostAddress, 5800, true)
  }


  override fun run() {
    AnsiConsole.systemInstall()
    val prevLink = Deadeye.link
    Deadeye.link = linkFromStream(Deadeye.getCamera(id.first()).stream)
    println("Redirecting target data stream to ${Deadeye.link}")
    repeat(LINES) { println() }
    val cameras = id.map {
      Deadeye.getCamera(it).also { camera ->
        camera.enabled = true
      }
    }
    cameras.first().targetDataListener = this
    val terminal = TerminalBuilder.builder().jansi(true).system(true).build()
    terminal.enterRawMode()
    terminal.reader().read()
    cameras.forEach { it.enabled = false }
    Deadeye.link = prevLink
    terminal.close()
    AnsiConsole.systemUninstall()
  }
}

