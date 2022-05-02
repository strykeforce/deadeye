import { UploadOutlined } from "@ant-design/icons";
import { Button, Col, message, Row, Upload } from "antd";
import React from "react";
import { configImageUpload, enableCamera } from "../../../common/api";
import { messageOffset } from "../../../common/util";
import "../camera-controls.less";

const UploadConfigSubPane = (props) => {
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

const UploadControl = (props) => {
  const { unit, inum } = props;

  const handleChange = (info) => {
    if (info.file.status !== "uploading") {
      console.log(info.file, info.fileList);
    }
    if (info.file.status === "done") {
      configImageUpload(unit, inum, info.file.name);
      enableCamera(unit, inum, false);
      // noinspection JSIgnoredPromiseFromCall
      void message.success({
        content: `${info.file.name} file uploaded successfully`,
        style: {
          marginTop: messageOffset,
        },
      });
    } else if (info.file.status === "error") {
      // noinspection JSIgnoredPromiseFromCall
      void message.error({
        content: `${info.file.name} file upload failed.`,
        style: {
          marginTop: messageOffset,
        },
      });
    }
  };

  const url =
    "http://" + document.domain + ":" + window.location.port + "/upload";

  return (
    <>
      <Row className="capture-pane-control">
        <Col span={12} className="capture-pane-control__label">
          Target Image
        </Col>
        <Col span={12} className="capture-pane-control__control--left">
          <Upload
            action={url}
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
