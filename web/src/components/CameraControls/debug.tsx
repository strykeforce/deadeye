import React, { useEffect, useState } from "react";
import SyntaxHighlighter from "react-syntax-highlighter";
import { dark } from "react-syntax-highlighter/dist/esm/styles/hljs";
import { CaptureConfig, PipelineConfig, Units } from "../../common/models";
import { getCamera } from "../../common/util";

// import "./index.less";

type Props = {
  unit: string;
  inum: number;
  config: CaptureConfig | PipelineConfig;
  section: string;
};

const DebugPane = (props: Props): JSX.Element => {
  const [isLoaded, setIsLoaded] = useState(false);
  const [units, setUnits] = useState<Units>({});

  useEffect(() => {
    fetch("http://localhost:5000/json")
      .then((res) => res.json())
      .then(
        (result) => {
          setIsLoaded(true);
          setUnits(result);
        },
        (error) => console.log(error)
      );
  }, [props.unit, props.inum, props.config]);

  if (!isLoaded) {
    return <div>Loading...</div>;
  } else {
    const camera = getCamera(`${props.unit}${props.inum}`, units);
    let section;
    if (props.section === "capture") {
      section = camera ? camera.capture : {};
    } else if (props.section === "pipeline") {
      section = camera ? camera.pipeline : {};
    } else if (props.section === "filter") {
      section = camera ? camera.pipeline.filter : {};
    }
    return (
      <SyntaxHighlighter language="json" style={dark}>
        {JSON.stringify(section, null, 2)}
      </SyntaxHighlighter>
    );
  }
};

export default DebugPane;
