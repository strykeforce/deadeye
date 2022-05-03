import { Router } from "@gatsbyjs/reach-router";
import React, { useEffect, useState } from "react";
import { close, sendMessage, subscribeToUnitUpdates } from "../../common/api";
import CameraPage from "../../pages/CameraPage";
import DefaultPage from "../../pages/DefaultPage";
import "./app.less";
import SettingsPage from "../../pages/SettingsPage";

const App = () => {
  const [units, setUnits] = useState();

  useEffect(() => {
    const handleUnitsChange = (units) => {
      setUnits(JSON.parse(units));
    };

    subscribeToUnitUpdates(handleUnitsChange);
    return () => close();
  }, []);

  // If admin server connects to NetworkTables when we subscribe, the "message"
  // API handler will force a refresh, ensuring our cameras are displayed.
  useEffect(() => {
    setTimeout(() => {
      sendMessage("ohai");
    }, 500);
  }, []);

  // console.debug(units);

  const unitPages = units ? (
    <>
      <CameraPage path="/id/:id" units={units} />
      <SettingsPage path="/settings" units={units} />
    </>
  ) : null;

  return (
    <Router>
      <DefaultPage id={"Z9"} units={units} default />
      {unitPages}
    </Router>
  );
};

export default App;
