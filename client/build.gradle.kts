
plugins {
    `idea`
    `java-library`
}

group = "org.strykeforce"
version = "20.0.0"

repositories {
    jcenter()
    maven(url = "https://frcmaven.wpi.edu/artifactory/release/")
}

dependencies {
    val wpiVersion = "2020.3.2"
    val slf4jVersion = "1.7.30"
    // This dependency is used internally, and not exposed to consumers on their own compile classpath.
    implementation("com.squareup.moshi:moshi:1.9.3")
    implementation("edu.wpi.first.ntcore:ntcore-java:$wpiVersion")
    implementation("org.slf4j:slf4j-api:$slf4jVersion")
    implementation("org.jetbrains:annotations:20.0.0")


    // Use JUnit Jupiter API for testing.
    testImplementation("org.junit.jupiter:junit-jupiter-api:5.6.2")

    // Use JUnit Jupiter Engine for testing.
    testRuntimeOnly("org.junit.jupiter:junit-jupiter-engine:5.6.2")
    testRuntimeOnly("edu.wpi.first.ntcore:ntcore-jni:$wpiVersion:osxx86-64")
    testRuntimeOnly("edu.wpi.first.wpiutil:wpiutil-java:$wpiVersion")
    testRuntimeOnly("org.slf4j:slf4j-nop:$slf4jVersion")
}

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}

allprojects {
    tasks.withType(JavaCompile::class.java).all {
        options.compilerArgs.add("-Xlint:unchecked")
    }
}

val test by tasks.getting(Test::class) {
    // Use junit platform for unit tests
    useJUnitPlatform()
}

idea {
    module {
        isDownloadJavadoc = true
        isDownloadSources = true
    }
}