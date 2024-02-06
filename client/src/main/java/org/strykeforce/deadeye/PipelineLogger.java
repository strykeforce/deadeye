package org.strykeforce.deadeye;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import edu.wpi.first.networktables.*;

import java.io.IOException;
import java.util.EnumSet;

class PipelineLogger {

    private final JsonAdapter<Message> jsonAdapter;
    @SuppressWarnings("FieldCanBeLocal")
    private final StringSubscriber sub;
    PipelineLogger(NetworkTable networkTable) {
        Moshi moshi = new Moshi.Builder().build();
        jsonAdapter = moshi.adapter(Message.class);
        sub = networkTable.getStringTopic("Log").subscribe("");

        networkTable.getInstance().addListener(sub,
                EnumSet.of(NetworkTableEvent.Kind.kValueAll),
                event -> valueChanged(event.valueData.value.getString()));
    }

    public void valueChanged(String value) {
        Message message = null;
        try {
            message = jsonAdapter.fromJson(value);
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
