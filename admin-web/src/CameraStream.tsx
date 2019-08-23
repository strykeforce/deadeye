import React, { useEffect, useRef } from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Paper from '@material-ui/core/Paper';
import Typography from '@material-ui/core/Typography';
import FormLabel from '@material-ui/core/FormLabel';
import FormControl from '@material-ui/core/FormControl';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Radio from '@material-ui/core/Radio';
import RadioGroup from '@material-ui/core/RadioGroup';
import standBy from './deadeye.png';
import { Camera } from './models';
import { configStream } from './api';

interface Props {
  camera: Camera;
  label?: string;
}

const CameraStream = (props: Props): JSX.Element => {
  const { camera, label } = props;
  const stream = camera.stream;

  const classes = useStyles();
  const imgRef = useRef<HTMLImageElement>(null);

  // useEffect(() => {
  //   if (camera.on && stream.view !== 'none') {
  //     console.log('starting camera timeout...');
  //     const timeoutId = window.setTimeout(() => {
  //       if (imgRef.current) {
  //         imgRef.current.src = stream.url;
  //       } else {
  //         throw new ReferenceError();
  //       }
  //     }, 500);
  //     return () => clearTimeout(timeoutId);
  //   }
  // }, [camera.on, stream]);

  const handleChange = (name: string) => (event: React.ChangeEvent<{}>, value: string): void => {
    event.preventDefault();
    const newStream = Object.assign(stream, { [name]: value });
    configStream(camera.unit, camera.inum, newStream);
  };

  let image;
  if (camera.on && stream.view !== 'none') image = stream.url;
  else image = standBy;

  return (
    <Paper className={classes.root}>
      {label && <Typography variant="body1">{label}</Typography>}
      <img src={image} ref={imgRef} width={width} className={classes.stream} alt="Stream" />
      <div className={classes.root}>
        <FormControl component="fieldset" className={classes.formControl}>
          <FormLabel component="legend">View</FormLabel>
          <RadioGroup name="view" className={classes.group} value={stream.view} onChange={handleChange('view')}>
            <FormControlLabel value="none" control={<Radio />} label="None" />
            <FormControlLabel value="original" control={<Radio />} label="Original" />
            <FormControlLabel value="mask" control={<Radio />} label="Mask" />
          </RadioGroup>
        </FormControl>
        <FormControl component="fieldset" className={classes.formControl}>
          <FormLabel component="legend">Contours</FormLabel>
          <RadioGroup
            name="contour"
            className={classes.group}
            value={stream.contour}
            onChange={handleChange('contour')}
          >
            <FormControlLabel value="none" control={<Radio />} label="None" />
            <FormControlLabel value="filter" control={<Radio />} label="Filtered" />
            <FormControlLabel value="all" control={<Radio />} label="All" />
          </RadioGroup>
        </FormControl>
      </div>
    </Paper>
  );
};

export default CameraStream;

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
