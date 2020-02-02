package org.strykeforce.deadeye

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

/**
 * MinAreaRectTargetData contains targeting data from Deadeye daemon's MinAreaRectPipelin, a pipeline that uses a
 * minimum-area (rotated) rectangle around the target contour.
 */
@JsonClass(generateAdapter = true)
class MinAreaRectTargetData(
    id: String = "NA",
    sn: Int = 0,
    valid: Boolean = false,
    @Json(name = "blx") val bottomLeftX: Double = 0.0,
    @Json(name = "bly") val bottomLeftY: Double = 0.0,
    @Json(name = "tlx") val topLeftX: Double = 0.0,
    @Json(name = "tly") val topLeftY: Double = 0.0,
    @Json(name = "trx") val topRightX: Double = 0.0,
    @Json(name = "try") val topRightY: Double = 0.0,
    @Json(name = "brx") val bottomRightX: Double = 0.0,
    @Json(name = "bry") val bottomRightY: Double = 0.0,
    @Json(name = "x") val centerOffsetX: Double = 0.0,
    @Json(name = "y") val centerOffsetY: Double = 0.0,
    @Json(name = "w") val width: Double = 0.0,
    @Json(name = "h") val height: Double = 0.0,
    @Json(name = "a") val angle: Double = 0.0
) : TargetData(id, sn, valid)
