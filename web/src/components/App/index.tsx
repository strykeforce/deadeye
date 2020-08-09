import React, { useEffect, useState } from "react";
import { close, subscribe, sendMessage } from "../../common/api";
import PageContainer from "../PageContainer";
import "./app.less";

const App = () => {
  const [units, setUnits] = useState({});

  useEffect(() => {
    const handleUnitsChange = (units: string): void => {
      setUnits(JSON.parse(units));
    };

    subscribe(handleUnitsChange);
    return () => close();
  }, []);

  // If admin server connects to NetworkTables when we subscribe, the "message"
  // API handler will force a refresh, ensuring our cameras are displayed.
  useEffect(() => {
    setTimeout(() => {
      sendMessage("ohai");
    }, 500);
  }, []);

  return <PageContainer units={units} />;
};

export default App;
