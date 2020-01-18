plugins {
  // Apply the Kotlin JVM plugin to add support for Kotlin.
  id("org.jetbrains.kotlin.jvm") version "1.3.61"

  // Apply the application plugin to add support for building a CLI application.
  application

  idea
}

repositories {
  // Use jcenter for resolving dependencies.
  // You can declare any Maven/Ivy/file repository here.
  jcenter()
  maven(url = "https://frcmaven.wpi.edu/artifactory/release/")
}

dependencies {
  val wpiVersion = "2020.1.2"
  implementation("org.strykeforce:deadeye:20.0.0")
  implementation("ch.qos.logback:logback-classic:1.2.3")
  implementation("io.dropwizard.metrics:metrics-core:4.+")
  implementation("edu.wpi.first.ntcore:ntcore-java:$wpiVersion")
  implementation("edu.wpi.first.ntcore:ntcore-jni:$wpiVersion:osxx86-64")
  implementation("edu.wpi.first.wpiutil:wpiutil-java:$wpiVersion")
  implementation("org.jline:jline-terminal:3.13.3")
  implementation("org.fusesource.jansi:jansi:1.18")
  implementation("com.github.ajalt:clikt:2.3.0")
  implementation ("com.squareup.moshi:moshi:1.9.2")


  // Align versions of all Kotlin components
  implementation(platform("org.jetbrains.kotlin:kotlin-bom"))

  // Use the Kotlin JDK 8 standard library.
  implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk8")

  // Use the Kotlin test library.
  testImplementation("org.jetbrains.kotlin:kotlin-test")

  // Use the Kotlin JUnit integration.
  testImplementation("org.jetbrains.kotlin:kotlin-test-junit")
}

application {
  // Define the main class for the application
  mainClassName = "org.strykeforce.deadeye.cli.AppKt"
}

idea {
  module {
    isDownloadJavadoc = true
    isDownloadSources = true
  }
}
