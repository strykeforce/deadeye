import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableHead from '@material-ui/core/TableHead';
import TableRow from '@material-ui/core/TableRow';
import Paper from '@material-ui/core/Paper';
import { configPipeline } from './api';
import Level from './level';
import { Camera } from './models';

interface Props {
  camera: Camera;
}

const CameraCapture = ({ camera }: Props): JSX.Element => {
  const classes = useStyles();
  const capture = camera.capture;

  const handleLevelChange = (name: string) => (value: number) => {
    const newCapture = Object.assign(capture, { [name]: value });
    console.log(newCapture);
    // configPipeline(camera.unit, camera.inum, newCapture);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Capture Settings
      </Typography>

      <TableContainer>
        <Table>
          <TableHead>
            <TableRow>
              <TableCell>Parameter</TableCell>
              <TableCell>Value</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            <TableRow>
              <TableCell>Type</TableCell>
              <TableCell>{capture.type}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell>Exposure</TableCell>
              <TableCell>{capture.exp}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell>Capture Size</TableCell>
              <TableCell>
                {capture.cw}x{capture.ch}
              </TableCell>
            </TableRow>
            <TableRow>
              <TableCell>Output Size</TableCell>
              <TableCell>
                {capture.ow}x{capture.oh}
              </TableCell>
            </TableRow>
            <TableRow>
              <TableCell>Flip Mode</TableCell>
              <TableCell>{capture.flip}</TableCell>
            </TableRow>
          </TableBody>
        </Table>
      </TableContainer>
    </Paper>
  );
};

export default CameraCapture;

const useStyles = makeStyles(theme => ({
  root: {
    padding: theme.spacing(2),
    display: 'flex',
    overflow: 'auto',
    flexDirection: 'column',
  },
  slider: {
    adding: theme.spacing(1),
    width: 350,
  },
  input: {
    width: 42,
  },
}));
