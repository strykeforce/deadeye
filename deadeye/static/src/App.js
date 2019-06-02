import React, { useState, useEffect } from "react";
import CssBaseline from "@material-ui/core/CssBaseline";
import io from "socket.io-client";
import Dashboard from "./Dashboard.js";

function App() {
  const [units, setUnits] = useState({});

  useEffect(() => {
    function handleUnitsChange(status) {
      setUnits(JSON.parse(status));
    }
    const socket = io("http://" + document.domain + ":" + window.location.port);

    socket.on("connect", () =>
      socket.emit("my event", { data: "I'm connected!" })
    );
    socket.on("refresh", data => handleUnitsChange(data));
    return () => {
      socket.close();
    };
  }, []);

  return (
    <>
      <CssBaseline />
      <Dashboard units={units} />
    </>
  );
}

export default App;
