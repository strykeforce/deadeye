package org.strykeforce.deadeye

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

/**
 * UprightTargetData contains targeting data from Deadeye daemon's UprightRectPipeline, a pipeline that uses an upright
 * bounding box around the target contour.
 */
@JsonClass(generateAdapter = true)
class UprightTargetData(
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
