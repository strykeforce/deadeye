import React, { useState, useEffect } from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Grid from '@material-ui/core/Grid';
import Typography from '@material-ui/core/Typography';
import Slider from '@material-ui/core/Slider';
import Input from '@material-ui/core/Input';
import { useDebounce } from './util';

interface Props {
  label: string;
  initialRange: number[];
  onRangeChange: (range: number[]) => void;
}

const Range = (props: Props): JSX.Element => {
  const { label, initialRange, onRangeChange } = props;
  const classes = useStyles();
  const [inputs, setInputs] = useState<string[]>(initialRange.map(String));
  const [range, setRange] = useState(initialRange);

  const debouncedRange = useDebounce(range, 500);
  useEffect(() => {
    if (debouncedRange[0] !== initialRange[0] || debouncedRange[1] !== initialRange[1]) {
      onRangeChange(debouncedRange);
    }
  }, [debouncedRange, onRangeChange, initialRange]);

  // Uncomment to allow input to apply on timeout
  // const debouncedInputs: string[] = useDebounce(inputs, 1000) as string[];
  // useEffect(() => {
  //   setRange([Number(debouncedInputs[0]), Number(debouncedInputs[1])]);
  // }, [debouncedInputs]);

  const handleSliderChange = (event: React.ChangeEvent<{}>, newValue: number | number[]): void => {
    setRange(newValue as number[]);
  };

  const handleLowerInputChange = (event: React.ChangeEvent<HTMLTextAreaElement | HTMLInputElement>): void => {
    setInputs([event.target.value, inputs[1]]);
  };

  const handleUpperInputChange = (event: React.ChangeEvent<HTMLTextAreaElement | HTMLInputElement>): void => {
    setInputs([inputs[0], event.target.value]);
  };

  const handleBlur = (): void => {
    setInputs([String(range[0]), String(range[1])]);
  };

  const handleKeyDown = (input: string) => (e: React.KeyboardEvent<HTMLTextAreaElement | HTMLInputElement>): void => {
    if (e.key === 'Enter') {
      let newValue = Number((e.target as HTMLInputElement).value);
      if (input === 'lower') {
        if (newValue < 0) {
          newValue = 0;
        }
        setRange([newValue, Number(inputs[1])]);
        return;
      }
      // input === 'upper'
      if (newValue > 255) {
        newValue = 255;
      }
      setRange([Number(inputs[0]), newValue]);
    }
  };

  return (
    <div className={classes.root}>
      <Typography id="input-slider" gutterBottom>
        {label}
      </Typography>
      <Grid container spacing={2} alignItems="center">
        <Grid item>
          <Input
            className={classes.input}
            value={inputs[0]}
            margin="dense"
            onChange={handleLowerInputChange}
            onBlur={handleBlur}
            onKeyDown={handleKeyDown('lower')}
          />
        </Grid>
        <Grid item xs>
          <Slider value={range} min={0} max={255} onChange={handleSliderChange} />
        </Grid>
        <Grid item>
          <Input
            className={classes.input}
            value={inputs[1]}
            margin="dense"
            onChange={handleUpperInputChange}
            onBlur={handleBlur}
            onKeyDown={handleKeyDown('upper')}
          />
        </Grid>
      </Grid>
    </div>
  );
};

export default Range;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(1),
    width: 350,
  },
  input: {
    width: 42,
  },
}));
