import React, { useState, useEffect } from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Paper from '@material-ui/core/Paper';
import Typography from '@material-ui/core/Typography';
import FormLabel from '@material-ui/core/FormLabel';
import FormControl from '@material-ui/core/FormControl';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Radio from '@material-ui/core/Radio';
import RadioGroup from '@material-ui/core/RadioGroup';
import { Camera } from './models';

import standBy from './deadeye.png';

const width = 322;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
  },
  formControl: {
    margin: theme.spacing(3),
  },
  group: {
    margin: theme.spacing(1, 0),
  },
  stream: {
    border: '1px solid black',
  },
}));

// eslint-disable-next-line @typescript-eslint/no-use-before-define
type Props = { camera: Camera } & typeof defaultProps;
const defaultProps = Object.freeze({ displayName: false });

const CameraStream = (props: Props): JSX.Element => {
  let { camera, displayName } = props;
  const classes = useStyles();
  const [source, setSource] = useState(standBy);
  const [, setValue] = React.useState('original');

  useEffect(() => {
    if (camera.on) {
      console.log('starting camera timeout...');
      setTimeout(() => setSource(camera.streamUrl), 500);
    } else {
      setSource(standBy);
    }
  }, [camera.on, camera.streamUrl]);

  const handleChange = (event: React.ChangeEvent<{}>, value: string): void => {
    event.preventDefault();
    setValue(value);
    console.log(`radio value = ${value}`);
  };

  return (
    <Paper className={classes.root}>
      {displayName && <Typography variant="body1">Camera {camera.id}</Typography>}
      <img src={source} width={width} className={classes.stream} alt="Stream" />
      <div className={classes.root}>
        <FormControl component="fieldset" className={classes.formControl}>
          <FormLabel component="legend">View</FormLabel>
          <RadioGroup name="view" className={classes.group} value="original" onChange={handleChange}>
            <FormControlLabel value="none" control={<Radio />} label="None" />
            <FormControlLabel value="original" control={<Radio />} label="Original" />
            <FormControlLabel value="mask" control={<Radio />} label="Mask" />
          </RadioGroup>
        </FormControl>
        <FormControl component="fieldset" className={classes.formControl}>
          <FormLabel component="legend">Contours</FormLabel>
          <RadioGroup name="view" className={classes.group} value="none" onChange={handleChange}>
            <FormControlLabel value="none" control={<Radio />} label="None" />
            <FormControlLabel value="filter" control={<Radio />} label="Filtered" />
            <FormControlLabel value="all" control={<Radio />} label="All" />
          </RadioGroup>
        </FormControl>
      </div>
    </Paper>
  );
};

CameraStream.defaultProps = defaultProps;

export default CameraStream;
