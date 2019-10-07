package org.strykeforce.deadeye.cli

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.core.subcommands
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.options.flag
import com.github.ajalt.clikt.parameters.options.option
import edu.wpi.first.networktables.NetworkTableInstance
import org.strykeforce.deadeye.Deadeye
import java.util.concurrent.CountDownLatch

class App : CliktCommand() {
  private val verbose by option("--verbose", "-v").flag("--no-verbose")
  override fun run() {
    val connectedSignal = CountDownLatch(1)
    NetworkTableInstance.getDefault().apply {
      addLogger({ println("NetworkTables: ${it.message}") }, if (verbose) 20 else 30, 100)
      startClient("127.0.0.1")
      addConnectionListener({ connectedSignal.countDown() }, true)
    }
    connectedSignal.await()
  }
}

class Enable: CliktCommand() {
  private val id by argument()
  override fun run() {
    val camera = Deadeye.getCamera(id)
    camera.enabled = true
  }
}

class Disable: CliktCommand() {
  private val id by argument()
  override fun run() {
    val camera = Deadeye.getCamera(id)
    camera.enabled = false
  }
}

class Status : CliktCommand() {
  override fun run() {
    val fmt = "%-7s %s"
    Deadeye.cameras.forEach {
      println(fmt.format("Camera:", it.id))
      println(fmt.format("Status:", if (it.enabled) "ON" else "OFF", if (it.error) "ERROR" else ""))
      with(it.config) {
        println(fmt.format("Config:", "exposure=$exposure, hue=$hue, sat=$saturation, val=$value"))
      }
      with(it.stream) { println(fmt.format("Stream:", "contour=$contour, view=$view ($url)")) }
      println()
    }
  }
}

class Watch : CliktCommand() {
  override fun run() {
    println("Watching")
  }
}

fun main(args: Array<String>) = App().subcommands(Enable(), Disable(), Status(), Watch()).main(args)
