import React, { useEffect, useState } from "react";
import { close, subscribe } from "../../util/api";
import PageContainer from "../PageContainer";
import "./index.less";

const App = () => {
  const [units, setUnits] = useState({});

  useEffect(() => {
    const handleUnitsChange = (units: string): void => {
      setUnits(JSON.parse(units));
    };

    subscribe(handleUnitsChange);
    return () => close();
  }, []);
  return <PageContainer units={units} />;
};

export default App;
