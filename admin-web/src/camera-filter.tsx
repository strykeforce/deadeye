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
import { Camera } from './models';

interface Props {
  camera: Camera;
}

const CameraFilter = ({ camera }: Props): JSX.Element => {
  const classes = useStyles();
  const filter = camera.pipeline.filter;

  const handleLevelChange = (name: string) => (value: number) => {
    // const newFilter = Object.assign(filter, { [name]: value });
    // console.log(newFilter);
    // configPipeline(camera.unit, camera.inum, newfilter);
  };

  return (
    <Paper className={classes.root}>
      <Typography component="h2" variant="h6" color="inherit" noWrap>
        Contour Filter Settings
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
              <TableCell>Bounding Box Area</TableCell>
              <TableCell>
                {filter.area[0]} - {filter.area[1]}
              </TableCell>
            </TableRow>
            <TableRow>
              <TableCell>Fullness</TableCell>
              <TableCell>
                {filter.fullness[0]} - {filter.fullness[1]}
              </TableCell>
            </TableRow>
            <TableRow>
              <TableCell>Aspect Ratio</TableCell>
              <TableCell>
                {filter.aspect[0]} - {filter.aspect[1]}
              </TableCell>
            </TableRow>
          </TableBody>
        </Table>
      </TableContainer>
    </Paper>
  );
};

export default CameraFilter;

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
