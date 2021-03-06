import { Button, Checkbox, Form, Input, InputNumber, message } from "antd";
import React, { useEffect, useRef } from "react";
import {
  configLink,
  refreshLink,
  subscribeToLinkUpdates,
  unsubscribeFromLinkUpdates,
} from "../../common/api";
import { Link } from "../../common/models";
import { messageOffset } from "../../common/util";

const layout = {
  labelCol: { span: 4 },
  wrapperCol: { span: 20 },
};
const tailLayout = {
  wrapperCol: { offset: 4, span: 20 },
};

const LinkPane = () => {
  const [form] = Form.useForm();

  const link = useRef<Link>();
  useEffect(() => {
    const handleLinkChange = (json: string) => {
      const linkConfig: Link[] = JSON.parse(json);
      link.current = linkConfig[0];
      form.setFieldsValue(link.current);
    };

    subscribeToLinkUpdates(handleLinkChange);

    return () => unsubscribeFromLinkUpdates();
  });

  useEffect(() => {
    refreshLink();
  }, []);

  const onFinish = (values: any) => {
    configLink([values]);
    message.info({
      content: "Link configuration updated.",
      style: {
        marginTop: messageOffset,
      },
    });
  };

  const onReset = () => {
    link.current && form.setFieldsValue(link.current);
  };

  // console.debug("link", link);

  return (
    <Form {...layout} name="link" form={form} onFinish={onFinish}>
      <Form.Item label="Address" name="address" rules={[{ required: true }]}>
        <Input style={{ width: "150px" }} />
      </Form.Item>

      <Form.Item label="Port" name="port" rules={[{ required: true }]}>
        <InputNumber />
      </Form.Item>

      <Form.Item label="Enabled" name="enabled" valuePropName="checked">
        <Checkbox />
      </Form.Item>

      <Form.Item {...tailLayout}>
        <Button type="primary" htmlType="submit" style={{ marginRight: "8px" }}>
          Submit
        </Button>
        <Button htmlType="button" onClick={onReset}>
          Reset
        </Button>
      </Form.Item>
    </Form>
  );
};

export default LinkPane;
