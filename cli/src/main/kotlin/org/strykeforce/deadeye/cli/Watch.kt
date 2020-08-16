package org.strykeforce.deadeye.cli

import com.codahale.metrics.Meter
import com.codahale.metrics.MetricRegistry
import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.arguments.multiple
import org.fusesource.jansi.Ansi.Attribute.INTENSITY_FAINT
import org.fusesource.jansi.Ansi.ansi
import org.fusesource.jansi.AnsiConsole
import org.jline.terminal.TerminalBuilder
import org.jline.utils.InfoCmp
import org.strykeforce.deadeye.Deadeye
import org.strykeforce.deadeye.TargetData
import org.strykeforce.deadeye.TargetDataListener
import kotlin.concurrent.timer

private val metrics = MetricRegistry()

class Watch : CliktCommand() {

    private val ids by argument().multiple()

    override fun run() {
        AnsiConsole.systemInstall()
        println()
        println(ansi().fgBrightBlue().a("id  serial  tgt   fps  drop"))
        println(ansi().a("===========================").reset())

        val watchers = ids.map { Deadeye<TargetData>(it, TargetData::class.java) }.map(::Watcher)
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

        terminal.close()
        AnsiConsole.systemUninstall()
    }
}

class Watcher(val camera: Deadeye<TargetData>) : TargetDataListener<TargetData> {

    init {
        camera.targetDataListener = this
        camera.enabled = true
    }

    private var td = TargetData("Z0", 0, false)
    private var dropped = 0
    private val fpsMeter: Meter by lazy { metrics.meter(camera.id) }


    override fun onTargetData(data: TargetData) {
        if (data.serial - td.serial > 1) dropped += data.serial - td.serial
        td = data
        fpsMeter.mark()
    }

    fun output() {
        val id = ansi().fgBrightBlue().a(td.id).reset()
        val sn = ansi().a(INTENSITY_FAINT).a(td.serial.toString().padStart(6)).reset()
        val valid = if (td.valid) ansi().fgBrightGreen().a("Y").reset() else ansi().fgBrightRed().a("N").reset()
        val fps = ansi().fgBrightYellow().a("%5.1f".format(fpsMeter.oneMinuteRate)).reset()
        val d =
            ansi().let { if (dropped == 0) it.a(INTENSITY_FAINT) else it.fgBrightRed() }
                .a(dropped.toString().padStart(4))
                .reset()
        println("$id  $sn   $valid $fps  $d")
    }
}
