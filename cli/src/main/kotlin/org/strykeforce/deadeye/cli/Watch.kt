package org.strykeforce.deadeye.cli

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import org.fusesource.jansi.Ansi.ansi
import org.fusesource.jansi.AnsiConsole
import org.jline.terminal.TerminalBuilder
import org.strykeforce.deadeye.Deadeye
import org.strykeforce.deadeye.TargetData
import org.strykeforce.deadeye.TargetDataListener

private const val LINES = 4

class Watch : CliktCommand(), TargetDataListener {

  private val id by argument()

  override fun onTargetData(data: TargetData) {
    println(ansi().cursorUpLine(LINES).cursorToColumn(0).a("Serial: ${data.sn}"))
    println(ansi().cursorToColumn(0).a(" Valid: ${data.valid}"))
    println(ansi().cursorToColumn(0).a("     X: ${data.x}"))
    println(ansi().cursorToColumn(0).a("     Y: ${data.y}"))
  }

  override fun run() {
    AnsiConsole.systemInstall()
    repeat(LINES) { println() }
    val camera = Deadeye.getCamera(id).also {
      it.targetDataListener = this
      it.enabled = true
    }
    val terminal = TerminalBuilder.builder().jansi(true).system(true).build()
    terminal.enterRawMode()
    terminal.reader().read()
    camera.enabled = false
    terminal.close()
    AnsiConsole.systemUninstall()
  }
}

