import { Router } from "@reach/router";
import React, { useEffect, useState } from "react";
import { close, sendMessage, subscribeToUnitUpdates } from "../../common/api";
import { Units } from "../../common/models";
import CameraPage from "../../pages/CameraPage";
import DefaultPage from "../../pages/DefaultPage";
import "./app.less";
import SettingsPage from "../../pages/SettingsPage";

const App = () => {
  const [units, setUnits] = useState<Units>();

  useEffect(() => {
    const handleUnitsChange = (units: string): void => {
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

  let unitPages;
  if (units) {
    unitPages = (
      <>
        <CameraPage path="/id/:id" units={units} />
        <SettingsPage path="/settings" units={units} />
      </>
    );
  }

  return (
    <Router>
      <DefaultPage units={units} default />
      {unitPages}
    </Router>
  );
};

export default App;
