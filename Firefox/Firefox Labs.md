
## 设置

- sidebar.revamp
左侧栏改造

## browser\components\sidebar\sidebar-customize.mjs
- render
```js
  render() {
    let extensions = this.getWindow().SidebarController.getExtensions();
    this.getWindow().SidebarController.getTools();
  }
```

## browser\components\sidebar\browser-sidebar.js
```js
// 初始化
  get sidebars() {
    if (this._sidebars) {
      return this._sidebars;
    }

    this._sidebars = new Map([
      [
        "viewHistorySidebar",
        this.makeSidebar({
          elementId: "sidebar-switcher-history",
          url: this.sidebarRevampEnabled
            ? "chrome://browser/content/sidebar/sidebar-history.html"
            : "chrome://browser/content/places/historySidebar.xhtml",
          menuId: "menu_historySidebar",
          triggerButtonId: "appMenuViewHistorySidebar",
          keyId: "key_gotoHistory",
          menuL10nId: "menu-view-history-button",
          revampL10nId: "sidebar-menu-history-label",
          iconUrl: "chrome://browser/content/firefoxview/view-history.svg",
        }),
      ],
      [
        "viewTabsSidebar",
        this.makeSidebar({
          elementId: "sidebar-switcher-tabs",
          url: this.sidebarRevampEnabled
            ? "chrome://browser/content/sidebar/sidebar-syncedtabs.html"
            : "chrome://browser/content/syncedtabs/sidebar.xhtml",
          menuId: "menu_tabsSidebar",
          classAttribute: "sync-ui-item",
          menuL10nId: "menu-view-synced-tabs-sidebar",
          revampL10nId: "sidebar-menu-synced-tabs-label",
          iconUrl: "chrome://browser/content/firefoxview/view-syncedtabs.svg",
        }),
      ],
      [
        "viewBookmarksSidebar",
        this.makeSidebar({
          elementId: "sidebar-switcher-bookmarks",
          url: "chrome://browser/content/places/bookmarksSidebar.xhtml",
          menuId: "menu_bookmarksSidebar",
          keyId: "viewBookmarksSidebarKb",
          menuL10nId: "menu-view-bookmarks",
          revampL10nId: "sidebar-menu-bookmarks-label",
          iconUrl: "chrome://browser/skin/bookmark-hollow.svg",
          disabled: true,
        }),
      ],
    ]);

    this.registerPrefSidebar(
      "browser.ml.chat.enabled",
      "viewGenaiChatSidebar",
      {
        elementId: "sidebar-switcher-genai-chat",
        url: "chrome://browser/content/genai/chat.html",
        menuId: "menu_genaiChatSidebar",
        menuL10nId: "menu-view-genai-chat",
        // Bug 1900915 to expose as conditional tool
        revampL10nId: "sidebar-menu-genai-chat-label",
        iconUrl: "chrome://mozapps/skin/extensions/category-discover.svg",
      }
    );

    if (!this.sidebarRevampEnabled) {
      this.registerPrefSidebar(
        "browser.megalist.enabled",
        "viewMegalistSidebar",
        {
          elementId: "sidebar-switcher-megalist",
          url: "chrome://global/content/megalist/megalist.html",
          menuId: "menu_megalistSidebar",
          menuL10nId: "menu-view-megalist-sidebar",
          revampL10nId: "sidebar-menu-megalist",
        }
      );
    } else {
      this._sidebars.set("viewCustomizeSidebar", {
        url: "chrome://browser/content/sidebar/sidebar-customize.html",
        revampL10nId: "sidebar-menu-customize-label",
        iconUrl: "chrome://browser/skin/preferences/category-general.svg",
      });
    }

    return this._sidebars;
  },

  /**
   * Retrieve the list of registered browser extensions.
   *
   * @returns {Array}
   */
  getExtensions() {
    const extensions = [];
    for (const [commandID, sidebar] of this.sidebars.entries()) {
      if (Object.hasOwn(sidebar, "extensionId")) {
        extensions.push({
          commandID,
          view: commandID,
          extensionId: sidebar.extensionId,
          iconUrl: sidebar.iconUrl,
          tooltiptext: sidebar.label,
          disabled: false,
        });
      }
    }
    return extensions;
  },

  /**
   * Retrieve the list of tools in the sidebar
   *
   * @returns {Array}
   */
  getTools() {
    return Object.keys(defaultTools).map(commandID => {
      const sidebar = this.sidebars.get(commandID);
      const disabled = !this.sidebarRevampTools
        .split(",")
        .includes(defaultTools[commandID]);
      return {
        commandID,
        view: commandID,
        iconUrl: sidebar.iconUrl,
        l10nId: sidebar.revampL10nId,
        disabled,
        // Reflect the current tool state defaulting to visible
        get hidden() {
          return !(sidebar.visible ?? true);
        },
      };
    });
  },
```
