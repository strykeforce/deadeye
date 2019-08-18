import React, { useState, useEffect } from 'react';
import CssBaseline from '@material-ui/core/CssBaseline';
import Dashboard from './Dashboard';
import { subscribe, close } from './api';

// FIXME: FC
const App: React.FC<{}> = () => {
  const [units, setUnits] = useState({});

  useEffect(() => {
    const handleUnitsChange = (units: string): void => setUnits(JSON.parse(units));

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
};

export default App;