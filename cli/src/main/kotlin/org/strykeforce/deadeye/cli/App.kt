package org.strykeforce.deadeye.cli

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.core.subcommands
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.options.default
import com.github.ajalt.clikt.parameters.options.flag
import com.github.ajalt.clikt.parameters.options.option
import edu.wpi.first.networktables.NetworkTableInstance
import org.strykeforce.deadeye.Deadeye
import org.strykeforce.deadeye.TargetData
import java.util.concurrent.CountDownLatch

class App : CliktCommand() {
  private val verbose by option("--verbose", "-v").flag("--no-verbose")
  private val nt by option("--nt-server", "-n").default("10.27.67.2")
  override fun run() {
    val connectedSignal = CountDownLatch(1)
    var connectionListener = 0
    NetworkTableInstance.getDefault().apply {
      addLogger({ println("NetworkTables: ${it.message}") }, if (verbose) 20 else 30, 100)
      startClient(nt)
      connectionListener = addConnectionListener({ connectedSignal.countDown() }, true)
    }
    connectedSignal.await()
    NetworkTableInstance.getDefault().removeConnectionListener(connectionListener)
  }
}

class Enable : CliktCommand() {
  private val id by argument()
  override fun run() {
    val camera = Deadeye.getCamera<TargetData>(id)
    camera.enabled = true
  }
}

class Disable : CliktCommand() {
  private val id by argument()
  override fun run() {
    val camera = Deadeye.getCamera<TargetData>(id)
    camera.enabled = false
  }
}

class Status : CliktCommand() {
  override fun run() {
    println("Deadeye Status")
    println("==============")
    val fmt = "%-7s %s"
    with(Deadeye.config) {
      println(fmt.format("Link:", "$address:$port"))
    }
    println()

//    Deadeye.cameras.forEach {
//      println(fmt.format("Camera:", it.id))
//      println(fmt.format("Status:", if (it.enabled) "ON" else "OFF", if (it.error) "ERROR" else ""))
//      with(it.config) {
//        println(fmt.format("Config:", "exposure=$exposure, hue=$hue, sat=$saturation, val=$value"))
//      }
//      with(it.stream) { println(fmt.format("Stream:", "contour=$contour, view=$view ($url)")) }
//      println()
//    }
  }
}

fun main(args: Array<String>) = App().subcommands(Enable(), Disable(), Status(), Watch()).main(args)
