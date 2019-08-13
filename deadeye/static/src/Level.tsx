import React, { useState, useEffect } from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Grid from '@material-ui/core/Grid';
import Typography from '@material-ui/core/Typography';
import Slider from '@material-ui/core/Slider';
import Input from '@material-ui/core/Input';
import { useDebounce } from './util';

interface Props {
  label: string;
  initialLevel: number;
  onLevelChange: (level: number) => void;
}

const Range = (props: Props): JSX.Element => {
  const { label, initialLevel, onLevelChange } = props;
  const classes = useStyles();
  const [level, setLevel] = useState(initialLevel);

  const debouncedLevel = useDebounce(level, 500) as number;

  useEffect(() => {
    if (debouncedLevel !== initialLevel) {
      onLevelChange(debouncedLevel);
    }
  }, [debouncedLevel, onLevelChange, initialLevel]);

  const handleSliderChange = (event: React.ChangeEvent<{}>, newValue: number | number[]): void => {
    setLevel(newValue as number);
  };

  const handleInputChange = (event: React.ChangeEvent<HTMLTextAreaElement | HTMLInputElement>): void => {
    const newValue = event.target.value === '' ? 0 : Number(event.target.value);
    setLevel(newValue);
  };

  const handleBlur = (): void => {
    if (level < 0) {
      setLevel(0);
    }
    if (level > 100) {
      setLevel(100);
    }
  };

  return (
    <div className={classes.root}>
      <Typography id="input-slider" gutterBottom>
        {label}
      </Typography>
      <Grid container spacing={2} alignItems="center">
        <Grid item xs>
          <Slider value={level} min={0} max={100} onChange={handleSliderChange} />
        </Grid>
        <Grid item>
          <Input
            className={classes.input}
            value={level}
            margin="dense"
            onChange={handleInputChange}
            onBlur={handleBlur}
            inputProps={{
              step: 1,
              min: 0,
              max: 100,
              type: 'number',
            }}
          />
        </Grid>
      </Grid>
    </div>
  );
};

export default Range;

const useStyles = makeStyles(theme => ({
  root: {
    adding: theme.spacing(1),
    width: 341,
  },
  input: {
    width: 42,
  },
}));
