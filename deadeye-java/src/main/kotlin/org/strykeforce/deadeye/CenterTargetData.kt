package org.strykeforce.deadeye

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

@JsonClass(generateAdapter = true)
class CenterTargetData(
    id: String = "NA",
    sn: Int = 0,
    valid: Boolean = false,
    @Json(name = "tx") val topLeftX: Int = 0,
    @Json(name = "ty") val topLeftY: Int = 0,
    @Json(name = "bx") val bottomRightX: Int = 0,
    @Json(name = "by") val bottomRightY: Int = 0,
    @Json(name = "x") val centerOffsetX: Int = 0,
    @Json(name = "y") val centerOffsetY: Int = 0
) : TargetData(id, sn, valid)
