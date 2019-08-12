import React, { useState, useEffect } from 'react';
import CssBaseline from '@material-ui/core/CssBaseline';
import Dashboard from './Dashboard.js';
import { subscribe, close } from './api';

function App() {
  const [units, setUnits] = useState({});

  useEffect(() => {
    function handleUnitsChange(units) {
      setUnits(JSON.parse(units));
    }

    subscribe(handleUnitsChange);

    return () => {
      close();
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
