package org.strykeforce.deadeye

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

@JsonClass(generateAdapter = true)
class CenterTargetData(
    id: String = "NA",
    sn: Int = 0,
    valid: Boolean = false,
    @Json(name = "tx") val tlx: Int = 0,
    @Json(name = "ty") val tly: Int = 0,
    @Json(name = "bx") val brx: Int = 0,
    @Json(name = "by") val bry: Int = 0,
    val x: Int = 0,
    val y: Int = 0
) : TargetData(id, sn, valid)
