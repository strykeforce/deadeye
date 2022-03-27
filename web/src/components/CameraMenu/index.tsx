import { SettingOutlined, VideoCameraOutlined } from "@ant-design/icons";
import { useNavigate } from "@reach/router";
import { Menu } from "antd";
import React, { useState } from "react";
import { Id } from "../../common/models";
import "./camera-menu.less";
import logo from "./deadeye.png";

type Props = {
  id?: string;
  ids: string[];
};

const CameraMenu = (props: Props) => {
  const { id, ids } = props;
  const [selectedId, setSelectedId] = useState<Id>(id);
  const navigate = useNavigate();

  const handleClick = (id: string) => {
    setSelectedId(id);
    // noinspection JSIgnoredPromiseFromCall
    navigate(`/id/${id}`);
  };

  const menuItems = ids.map((id) => (
    <Menu.Item
      key={id}
      icon={<VideoCameraOutlined />}
      onClick={() => handleClick(id)}
    >
      {id}
    </Menu.Item>
  ));

  return (
    <>
      <div className="camera-menu__logo">
        <img src={logo} alt="Logo" />
      </div>
      <Menu
        className="camera-menu"
        theme="dark"
        mode="inline"
        selectedKeys={selectedId ? [selectedId] : undefined}
      >
        {menuItems}
        <Menu.Item
          key="settings"
          icon={<SettingOutlined />}
          onClick={() => navigate("/settings")}
        />
      </Menu>
    </>
  );
};

export default CameraMenu;
