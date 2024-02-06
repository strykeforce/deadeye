package org.strykeforce.deadeye;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableValue;

import java.io.IOException;

class PipelineLogger {

  private final JsonAdapter<Message> jsonAdapter;

  PipelineLogger(NetworkTable networkTable) {
    Moshi moshi = new Moshi.Builder().build();
    jsonAdapter = moshi.adapter(Message.class);
  }

  public void valueChanged(NetworkTable table, String key, NetworkTableEntry entry,
      NetworkTableValue value, int flags) {
    if (!value.isString()) {
      return;
    }

    Message message = null;
    try {
      message = jsonAdapter.fromJson(value.getString());
    } catch (IOException e) {
      Deadeye.logger.error("PipelineLogger message parse error", e);
    }

    if (message == null || message.level == null) {
      return;
    }

    switch (message.level) {
      case "debug":
        Deadeye.logger.debug(message.message);
        break;
      case "info":
        Deadeye.logger.info(message.message);
        break;
      case "warn":
        Deadeye.logger.warn(message.message);
        break;
      case "error":
        Deadeye.logger.error(message.message);
        break;
      default:
        Deadeye.logger.error("Unexpected value: {}:{}", message.level, message.message);
    }
  }

  static class Message {

    final String level;
    final String message;

    public Message(String level, String message) {
      this.level = level;
      this.message = message;
    }
  }

}
