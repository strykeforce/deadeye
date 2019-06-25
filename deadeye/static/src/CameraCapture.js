import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Grid from "@material-ui/core/Grid";
import Input from "@material-ui/core/Input";
import Slider from "@material-ui/lab/Slider";
import Typography from "@material-ui/core/Typography";
import Paper from "@material-ui/core/Paper";

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
    display: "flex",
    overflow: "auto",
    flexDirection: "column"
  },
  slider: {
    adding: theme.spacing(1),
    width: 350
  },
  input: {
    width: 42
  }
}));

export default function CameraCapture({ camera }) {
  const classes = useStyles();

  const handleSliderChange = value => {
    console.log(`handleSliderChange: ${value}`);
  };

  const handleBlur = () => {};

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Capture Settings
      </Typography>
      <div className={classes.slider}>
        <Typography id="input-slider" gutterBottom>
          Exposure
        </Typography>
        <Grid container spacing={2} alignItems="center">
          <Grid item xs>
            <Slider
              value={10}
              min={0}
              max={255}
              onChange={handleSliderChange}
            />
          </Grid>
          <Grid item>
            <Input
              className={classes.input}
              value={10}
              margin="dense"
              onChange={null}
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
    </Paper>
  );
}
