import React, { ReactNode } from 'react';
import Typography from '@material-ui/core/Typography';

interface Props {
  children: ReactNode;
}

const Title = (props: Props): JSX.Element => {
  return (
    <Typography component="h2" variant="h6" color="primary" gutterBottom>
      {props.children}
    </Typography>
  );
};

export default Title;
