import React from "react";
import clsx from "clsx";
import { makeStyles } from "@material-ui/core/styles";
import Container from "@material-ui/core/Container";
import Grid from "@material-ui/core/Grid";
import Paper from "@material-ui/core/Paper";
import { get } from "./util";

const useStyles = makeStyles(theme => ({
  container: {
    paddingTop: theme.spacing(4),
    paddingBottom: theme.spacing(4)
  },
  paper: {
    padding: theme.spacing(2),
    display: "flex",
    overflow: "auto",
    flexDirection: "column"
  },
  fixedHeight: {
    height: 240
  }
}));

export default function Unit({ units, selectedId }) {
  const classes = useStyles();
  const fixedHeightPaper = clsx(classes.paper, classes.fixedHeight);
  const unitId = selectedId.charAt(0);
  const getUnitId = get([unitId, "id"]);

  return (
    <Container maxWidth="lg" className={classes.container}>
      <Grid container spacing={3}>
        {/* Chart */}
        <Grid item xs={"auto"}>
          <Paper className={fixedHeightPaper}>
            <ul>
              <li>Unit {getUnitId(units)}</li>
              <li>Unit {get([unitId, "id"])(units)}</li>
            </ul>
          </Paper>
        </Grid>
      </Grid>
    </Container>
  );
}
