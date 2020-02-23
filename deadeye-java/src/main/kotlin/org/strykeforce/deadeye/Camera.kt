package org.strykeforce.deadeye

import com.squareup.moshi.Json
import com.squareup.moshi.JsonAdapter
import com.squareup.moshi.JsonClass
import okio.Buffer

interface Camera<T : TargetData> {
    val id: String
    var enabled: Boolean
    var lightEnabled: Boolean
    val error: Boolean
    val capture: Capture
    val pipeline: Pipeline
    val stream: Stream
    var targetDataListener: TargetDataListener?
    var jsonAdapter: JsonAdapter<T>
    fun parse(buffer: Buffer)

    @JsonClass(generateAdapter = true)
    data class Capture(
        @Json(name = "exp") val exposure: Double, @Json(name = "cw") val captureWidth: Int, @Json(name = "ch") val captureHeight: Int,
        val flip: Int,
        val fps: Int, @Json(name = "ow") val outputWidth: Int, @Json(name = "oh") val outputHeight: Int,
        val type: String
    )

    @JsonClass(generateAdapter = true)
    data class Filter(
        val area: List<Double>, val fullness: List<Double>, val aspect: List<Double>
    )

    @JsonClass(generateAdapter = true)
    data class Pipeline(
        @Json(name = "sn") val serial: Int,
        val hue: List<Int>, @Json(name = "sat") val saturation: List<Int>, @Json(name = "val") val value: List<Int>,
        val filter: Filter
    )

    @JsonClass(generateAdapter = true)
    data class Stream(@Json(name = "sn") val serial: Int, val contour: String, val view: String, val url: String)
}