import io from "socket.io-client";
const socket = io("http://" + document.domain + ":" + window.location.port);
socket.on("connect", () => socket.emit("my event", { data: "I'm connected!" }));
socket.on("refresh", data => console.log(JSON.parse(data)));

export const addChangeListener = () => {};
