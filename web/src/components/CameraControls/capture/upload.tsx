import { UploadOutlined } from "@ant-design/icons";
import { Button, Col, message, Row, Upload } from "antd";
import { UploadChangeParam } from "antd/es/upload";
import React from "react";
import "../camera-controls.less";
import { CaptureControlProps } from "../capture-pane";
import {
  configCapture,
  enableCamera,
  configImageUpload,
} from "../../../common/api";

const UploadConfigSubPane = (props: CaptureControlProps) => {
  return (
    <>
      <Row>
        <Col span={24}>
          <UploadControl {...props} />
        </Col>
      </Row>
    </>
  );
};

export default UploadConfigSubPane;

const UploadControl = (props: CaptureControlProps) => {
  const { unit, inum } = props;

  const handleChange = (info: UploadChangeParam) => {
    if (info.file.status !== "uploading") {
      console.log(info.file, info.fileList);
    }
    if (info.file.status === "done") {
      configImageUpload(unit, inum, info.file.name);
      enableCamera(unit, inum, false);
      message.success({
        content: `${info.file.name} file uploaded successfully`,
        style: {
          marginTop: "8vh",
        },
      });
    } else if (info.file.status === "error") {
      message.error({
        content: `${info.file.name} file upload failed.`,
        style: {
          marginTop: "8vh",
        },
      });
    }
  };

  return (
    <>
      <Row className="capture-pane-control">
        <Col span={12} className="capture-pane-control__label">
          Target Image
        </Col>
        <Col span={12} className="capture-pane-control__control--left">
          <Upload
            action="http://localhost:5000/upload"
            accept="image/*"
            name="image"
            showUploadList={false}
            onChange={handleChange}
          >
            <Button>
              <UploadOutlined /> Upload
            </Button>
          </Upload>
        </Col>
      </Row>
      <Row className="capture-pane-control">
        <Col span={24} className="capture-pane-control__label">
          Note: If using multiple Deadeye units, target image is uploaded to the
          Deadeye unit at the browser address above.
        </Col>
      </Row>
    </>
  );
};
