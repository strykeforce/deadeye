import React, { useState, useEffect } from "react";
import { makeStyles } from "@material-ui/core/styles";
import Grid from "@material-ui/core/Grid";
import Typography from "@material-ui/core/Typography";
import Slider from "@material-ui/lab/Slider";
import Input from "@material-ui/core/Input";
import { useDebounce } from "./util";

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(1),
    width: 350
  },
  input: {
    width: 42
  }
}));

export default function Range(props) {
  const classes = useStyles();
  const [range, setRange] = React.useState(props.range);

  const debouncedRange = useDebounce(range, 500);

  const onRangeChange = props.onRangeChange;
  useEffect(() => {
    if (
      debouncedRange[0] !== props.range[0] ||
      debouncedRange[1] !== props.range[1]
    ) {
      onRangeChange(debouncedRange);
    }
  }, [debouncedRange, onRangeChange, props.range]);

  const handleSliderChange = (event, newValue) => {
    setRange(newValue);
  };

  const handleLowerInputChange = event => {
    const newValue = event.target.value === "" ? 0 : Number(event.target.value);
    setRange([newValue, range[1]]);
  };

  const handleUpperInputChange = event => {
    const newValue = event.target.value === "" ? 0 : Number(event.target.value);
    setRange([range[0], newValue]);
  };

  const handleBlur = () => {
    if (range[0] < 0) {
      setRange([0, range[1]]);
    }
    if (range[1] > 255) {
      setRange([range[0], 255]);
    }
  };

  return (
    <div className={classes.root}>
      <Typography id="input-slider" gutterBottom>
        {props.label}
      </Typography>
      <Grid container spacing={2} alignItems="center">
        <Grid item>
          <Input
            type="number"
            className={classes.input}
            value={range[0]}
            margin="dense"
            onChange={handleLowerInputChange}
            onBlur={handleBlur}
            inputProps={{
              step: 1,
              min: 0,
              max: 255
            }}
          />
        </Grid>
        <Grid item xs>
          <Slider
            value={range}
            min={0}
            max={255}
            onChange={handleSliderChange}
          />
        </Grid>
        <Grid item>
          <Input
            className={classes.input}
            value={range[1]}
            margin="dense"
            onChange={handleUpperInputChange}
            onBlur={handleBlur}
            inputProps={{
              step: 1,
              min: 0,
              max: 255,
              type: "number"
            }}
          />
        </Grid>
      </Grid>
    </div>
  );
}
