import React from 'react';
import standBy from './deadeye.png';

interface Props {
  enabled: boolean;
  url: string;
}

const Image = (props: Props): JSX.Element => {
  const { enabled, url } = props;
  return (
    <img src={enabled ? url : standBy} height={242} width={322} style={{ border: '1px solid black' }} alt="Stream" />
  );
};

export default React.memo(Image);
