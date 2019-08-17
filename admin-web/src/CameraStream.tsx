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
import { StreamConfig } from './models';

interface Props {
  enable: boolean;
  stream: StreamConfig;
  label?: string;
}

const CameraStream = (props: Props): JSX.Element => {
  const { enable, stream, label } = props;

  const classes = useStyles();
  const [, setValue] = React.useState('original');
  const imgRef = useRef<HTMLImageElement>(null);

  useEffect(() => {
    if (enable) {
      console.log('starting camera timeout...');
      let timeoutId = setTimeout(() => {
        if (imgRef.current) {
          imgRef.current.src = stream.url;
        } else {
          throw new ReferenceError();
        }
      }, 500);
      return () => clearTimeout(timeoutId);
    }
  }, [enable, stream]);

  const handleChange = (event: React.ChangeEvent<{}>, value: string): void => {
    event.preventDefault();
    setValue(value);
    console.log(`radio value = ${value}`);
  };

  return (
    <Paper className={classes.root}>
      {label && <Typography variant="body1">{label}</Typography>}
      <img src={standBy} ref={imgRef} width={width} className={classes.stream} alt="Stream" />
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
