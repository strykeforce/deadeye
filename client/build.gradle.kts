plugins {
    idea
    `java-library`
    `maven-publish`
}

group = "org.strykeforce"
version = "21.3.0" // updated by scripts/bump.py

repositories {
    jcenter()
    maven(url = "https://frcmaven.wpi.edu/artifactory/release/")
    mavenCentral()
}

dependencies {
    val wpiVersion = "2022.2.1"
    val slf4jVersion = "1.7.33"
    val junitVersion = "5.8.2"
    // This dependency is used internally, and not exposed to consumers on their own compile classpath.
    implementation("com.squareup.moshi:moshi:1.13.0")
    implementation("edu.wpi.first.ntcore:ntcore-java:$wpiVersion")
    implementation("org.slf4j:slf4j-api:$slf4jVersion")
    implementation("org.jetbrains:annotations:22.0.0")


    // Use JUnit Jupiter API for testing.
    testImplementation("org.junit.jupiter:junit-jupiter-api:$junitVersion")
    testImplementation("org.junit.jupiter:junit-jupiter-params:$junitVersion")

    // Use JUnit Jupiter Engine for testing.
    testRuntimeOnly("org.junit.jupiter:junit-jupiter-engine:$junitVersion")
    testRuntimeOnly("edu.wpi.first.ntcore:ntcore-jni:$wpiVersion:osxx86-64")
    testRuntimeOnly("edu.wpi.first.ntcore:ntcore-jni:$wpiVersion:linuxx86-64")
    testRuntimeOnly("edu.wpi.first.wpiutil:wpiutil-java:$wpiVersion")
    testRuntimeOnly("ch.qos.logback:logback-classic:1.2.10")
}

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
    withSourcesJar()
    withJavadocJar()
}

allprojects {
    tasks.withType(JavaCompile::class.java).all {
        options.compilerArgs.add("-Xlint:unchecked")
    }
    tasks.withType(Javadoc::class.java).all {
        options {
            this as StandardJavadocDocletOptions
            links(
                "https://docs.oracle.com/en/java/javase/11/docs/api",
                "https://first.wpi.edu/wpilib/allwpilib/docs/release/java",
                "https://javadoc.io/doc/com.squareup.okio/okio/1.17.5/",
                "https://javadoc.io/doc/org.jetbrains/annotations/22.0.0/"
            )
        }
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

publishing {
    repositories {
        maven {
            url = uri("s3://maven.strykeforce.org/repo")
            credentials(AwsCredentials::class) {
                accessKey = System.getenv("STRYKEFORCE_AWS_KEY")
                secretKey = System.getenv("STRYKEFORCE_AWS_SECRET")
            }
        }
    }

    publications {
        create<MavenPublication>("deadeye") {
            from(components["java"])

            pom {
                name.set("Deadeye Client Library")
                description.set("Java client library for Stryke Force FRC Vision System")
                url.set("https://github.com/strykeforce/deadeye")

                licenses {
                    license {
                        name.set("MIT License")
                        url.set("https://github.com/strykeforce/deadeye/blob/master/LICENSE")
                    }
                }
            }
        }
    }
}
