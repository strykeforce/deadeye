package org.strykeforce.deadeye;

import okio.BufferedSource;

import java.io.IOException;

@FunctionalInterface
interface TargetDataHandler {
    void handleTargetData(BufferedSource source) throws IOException;
}
