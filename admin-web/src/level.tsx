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
  const [input, setInput] = useState(String(initialLevel));
  const [level, setLevel] = useState(initialLevel);
  const debouncedLevel = useDebounce(level, 500);

  useEffect(() => {
    if (debouncedLevel !== initialLevel) {
      onLevelChange(debouncedLevel);
    }
  }, [debouncedLevel, onLevelChange, initialLevel]);

  const handleSliderChange = (event: React.ChangeEvent<{}>, value: number | number[]): void => {
    const level = value as number;
    setLevel(level);
    setInput(String(level));
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLTextAreaElement | HTMLInputElement>): void => {
    if (e.key !== 'Enter') {
      return;
    }
    let newValue = Number((e.target as HTMLInputElement).value);
    if (newValue < 0) {
      newValue = 0;
    }
    if (newValue > 100) {
      newValue = 100;
    }
    setLevel(Number(newValue));
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
            value={input}
            margin="dense"
            onChange={(e) => setInput(e.target.value)}
            onBlur={() => setInput(String(level))}
            onKeyDown={handleKeyDown}
          />
        </Grid>
      </Grid>
    </div>
  );
};

export default Range;

const useStyles = makeStyles((theme) => ({
  root: {
    adding: theme.spacing(1),
    width: 341,
  },
  input: {
    width: 42,
  },
}));
