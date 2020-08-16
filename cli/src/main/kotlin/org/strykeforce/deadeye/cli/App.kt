package org.strykeforce.deadeye.cli

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.core.subcommands
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.options.default
import com.github.ajalt.clikt.parameters.options.flag
import com.github.ajalt.clikt.parameters.options.option
import edu.wpi.first.networktables.NetworkTableInstance
import mu.KotlinLogging
import org.strykeforce.deadeye.Deadeye
import org.strykeforce.deadeye.TargetData
import java.util.concurrent.CountDownLatch

private val logger = KotlinLogging.logger {}

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
        val camera = Deadeye<TargetData>(id, TargetData::class.java)
        camera.enabled = true
    }
}

class Disable : CliktCommand() {
    private val id by argument()
    override fun run() {
        val camera = Deadeye<TargetData>(id, TargetData::class.java)
        camera.enabled = false
    }
}
//
//class Dump : CliktCommand() {
//    private val id by argument()
//    override fun run() {
//        val camera = Deadeye.getCamera<TargetData>(id)
//        val sink = Buffer()
//        val writer = JsonWriter.of(sink).also { it.indent = "  " }
//        camera.toJson(writer)
//        sink.copyTo(System.out)
//    }
//}

//fun main(args: Array<String>) = App().subcommands(Enable(), Disable(), Dump(), Watch()).main(args)
fun main(args: Array<String>) = App().subcommands(Enable(), Disable(), Watch()).main(args)

//fun <T : TargetData> Camera<T>.toJson(writer: JsonWriter) {
//    val moshi = Moshi.Builder().build()
//    writer.beginObject()
//    writer.name("capture")
//    Camera_CaptureJsonAdapter(moshi).toJson(writer, this.capture)
//    writer.name("pipeline")
//    Camera_PipelineJsonAdapter(moshi).toJson(writer, this.pipeline)
//    writer.name("stream")
//    Camera_StreamJsonAdapter(moshi).toJson(writer, this.stream)
//    writer.endObject()
//}