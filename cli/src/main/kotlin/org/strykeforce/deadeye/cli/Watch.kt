package org.strykeforce.deadeye.cli

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.arguments.multiple
import org.fusesource.jansi.Ansi.ansi
import org.fusesource.jansi.AnsiConsole
import org.jline.terminal.TerminalBuilder
import org.strykeforce.deadeye.Deadeye
import org.strykeforce.deadeye.TargetData
import org.strykeforce.deadeye.TargetDataListener
import java.net.Inet4Address

private const val LINES = 4

class Watch : CliktCommand(), TargetDataListener {

  private val id by argument().multiple()

  override fun onTargetData(data: TargetData) {
    println(ansi().cursorUpLine(LINES).cursorToColumn(0).a("Serial: ${data.sn}"))
    println(ansi().cursorToColumn(0).a(" Valid: ${data.valid}"))
    println(ansi().cursorToColumn(0).a("     X: ${data.x}"))
    println(ansi().cursorToColumn(0).a("     Y: ${data.y}"))
  }

  override fun run() {
    AnsiConsole.systemInstall()
    val prevLink = Deadeye.link
    Deadeye.link = Deadeye.Link(Inet4Address.getLocalHost().hostAddress, 5800)
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

