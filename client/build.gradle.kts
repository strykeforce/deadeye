plugins {
    `idea`
    `java-library`
    `maven-publish`
}

group = "org.strykeforce"
version = "21.2.0" // updated by scripts/bump.py

repositories {
    jcenter()
    maven(url = "https://frcmaven.wpi.edu/artifactory/release/")
}

dependencies {
    val wpiVersion = "2021.3.1"
    val slf4jVersion = "1.7.30"
    val junitVersion = "5.7.2"
    // This dependency is used internally, and not exposed to consumers on their own compile classpath.
    implementation("com.squareup.moshi:moshi:1.11.0")
    implementation("edu.wpi.first.ntcore:ntcore-java:$wpiVersion")
    implementation("org.slf4j:slf4j-api:$slf4jVersion")
    implementation("org.jetbrains:annotations:20.1.0")


    // Use JUnit Jupiter API for testing.
    testImplementation("org.junit.jupiter:junit-jupiter-api:$junitVersion")

    // Use JUnit Jupiter Engine for testing.
    testRuntimeOnly("org.junit.jupiter:junit-jupiter-engine:$junitVersion")
    testRuntimeOnly("edu.wpi.first.ntcore:ntcore-jni:$wpiVersion:osxx86-64")
    testRuntimeOnly("edu.wpi.first.wpiutil:wpiutil-java:$wpiVersion")
    testRuntimeOnly("org.slf4j:slf4j-nop:$slf4jVersion")
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