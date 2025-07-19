#

使用tkinter作为GUI界面，支持MySQL数据库连接。

```shell
pip install mysql-connector
```

```python
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import mysql.connector
from mysql.connector import Error
import pandas as pd

class DatabaseManager:
    def __init__(self, root):
        self.root = root
        self.root.title("数据库管理工具")
        self.root.geometry("800x600")
        
        # 主容器配置
        self.main_frame = ttk.Frame(root)
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 连接参数区域
        self.setup_connection_ui()

        # 表格显示区域
        self.setup_table_ui()
        
        # 连接对象
        self.connection = None
    
    def setup_connection_ui(self):
        conn_frame = ttk.Frame(self.main_frame)
        conn_frame.pack(fill=tk.X, padx=5, pady=5)
        
        # 连接参数输入框（带默认值）
        ttk.Label(conn_frame, text="主机:").grid(row=0, column=0, sticky="e")
        self.host_entry = ttk.Entry(conn_frame)
        self.host_entry.insert(0, "localhost")
        self.host_entry.grid(row=0, column=1, sticky="ew")
        
        ttk.Label(conn_frame, text="用户名:").grid(row=1, column=0, sticky="e")
        self.user_entry = ttk.Entry(conn_frame)
        self.user_entry.insert(0, "root")
        self.user_entry.grid(row=1, column=1, sticky="ew")
        
        ttk.Label(conn_frame, text="密码:").grid(row=2, column=0, sticky="e")
        self.pwd_entry = ttk.Entry(conn_frame, show="*")
        self.pwd_entry.insert(0, "123456")
        self.pwd_entry.grid(row=2, column=1, sticky="ew")
        
        ttk.Label(conn_frame, text="数据库:").grid(row=3, column=0, sticky="e")
        self.db_entry = ttk.Entry(conn_frame)
        self.db_entry.insert(0, "yl_yunbrowser")
        self.db_entry.grid(row=3, column=1, sticky="ew")
        
        # 按钮区域
        btn_frame = ttk.Frame(conn_frame)
        btn_frame.grid(row=4, columnspan=2, pady=5)
        
        self.connect_btn = ttk.Button(btn_frame, text="连接", command=self.connect_db)
        self.connect_btn.pack(side=tk.LEFT, padx=5)
        
        self.disconnect_btn = ttk.Button(btn_frame, text="断开", command=self.disconnect_db, state=tk.DISABLED)
        self.disconnect_btn.pack(side=tk.LEFT, padx=5)
        
        self.reconnect_btn = ttk.Button(btn_frame, text="重连", command=self.reconnect_db, state=tk.DISABLED)
        self.reconnect_btn.pack(side=tk.LEFT, padx=5)

        self.export_btn = ttk.Button(btn_frame, text="导出表格",
                                   command=self.export_table_data,
                                   state=tk.DISABLED)
        self.export_btn.pack(side=tk.LEFT, padx=5)
        
        # 配置网格权重
        conn_frame.columnconfigure(1, weight=1)
    
    def setup_table_ui(self):
        table_frame = ttk.Frame(self.main_frame)
        table_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # 表格列表区域
        self.tables_list = tk.Listbox(table_frame)
        self.tables_list.pack(side=tk.LEFT, fill=tk.Y, padx=(0,5))
        self.tables_list.bind('<<ListboxSelect>>', self.show_table_data)
        
        # 创建表格容器
        tree_container = ttk.Frame(table_frame)
        tree_container.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # 创建垂直滚动条
        self.v_scroll = ttk.Scrollbar(tree_container)
        self.v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 创建水平滚动条
        self.h_scroll = ttk.Scrollbar(tree_container, orient=tk.HORIZONTAL)
        self.h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        
        # 创建Treeview
        self.tree = ttk.Treeview(
            tree_container,
            yscrollcommand=self.v_scroll.set,
            xscrollcommand=self.h_scroll.set
        )
        self.tree.pack(fill=tk.BOTH, expand=True)
        
        # 配置滚动条
        self.v_scroll.config(command=self.tree.yview)
        self.h_scroll.config(command=self.tree.xview)
    
    def connect_db(self):
        try:
            self.connection = mysql.connector.connect(
                host=self.host_entry.get(),
                user=self.user_entry.get(),
                password=self.pwd_entry.get(),
                database=self.db_entry.get()
            )
            self.show_tables()
            self.connect_btn.config(state=tk.DISABLED)
            self.disconnect_btn.config(state=tk.NORMAL)
            self.reconnect_btn.config(state=tk.NORMAL)
            self.export_btn.config(state=tk.NORMAL)
            messagebox.showinfo("成功", "数据库连接成功")
        except Error as e:
            messagebox.showerror("错误", f"连接失败: {e}")
    
    def disconnect_db(self):
        if self.connection:
            self.connection.close()
            self.connection = None
            self.tables_list.delete(0, tk.END)
            self.tree.delete(*self.tree.get_children())
            self.connect_btn.config(state=tk.NORMAL)
            self.disconnect_btn.config(state=tk.DISABLED)
            self.reconnect_btn.config(state=tk.DISABLED)
            self.export_btn.config(state=tk.DISABLED)
            messagebox.showinfo("成功", "已断开数据库连接")
    
    def reconnect_db(self):
        self.disconnect_db()
        self.connect_db()
    
    def show_tables(self):
        if self.connection:
            cursor = self.connection.cursor()
            cursor.execute("SHOW TABLES")
            tables = cursor.fetchall()
            self.tables_list.delete(0, tk.END)
            for table in tables:
                self.tables_list.insert(tk.END, table[0])
            cursor.close()

    def show_table_data(self, event):
        if not self.connection:
            return
            
        selected_table = self.tables_list.get(self.tables_list.curselection())
        if selected_table:
            cursor = self.connection.cursor()
            cursor.execute(f"SELECT * FROM {selected_table} LIMIT 1000")
            
            # 清空treeview
            self.tree.delete(*self.tree.get_children())
            self.tree["columns"] = [desc[0] for desc in cursor.description]
            
            # 设置列
            for col in self.tree["columns"]:
                self.tree.heading(col, text=col)
                self.tree.column(col, minwidth=100, anchor=tk.CENTER, stretch=True)
            
            # 添加数据
            for row in cursor:
                self.tree.insert("", tk.END, values=row)

            # 立即调整列宽
            self.adjust_column_widths()
            cursor.close()

    def adjust_column_widths(self, event=None):
            
        # 计算每列最大宽度
        for col in self.tree["columns"]:
            max_width = 100  # 最小宽度
            self.tree.heading(col, text=self.tree.heading(col)["text"])  # 刷新标题

            # 计算标题宽度
            heading_width = len(self.tree.heading(col)["text"]) * 8 + 20

            # 计算内容最大宽度
            content_width = max([len(str(self.tree.set(item, col))) * 8 for item in self.tree.get_children()] or [0])

            # 取最大值
            max_width = max(max_width, heading_width, content_width)
            print(f"Column {col} width: {max_width}")

            # 设置列宽
            self.tree.column(col, width=min(max_width, 300))  # 限制最大宽度300px

    def export_table_data(self):
        if not self.connection:
            return
            
        try:
            # 获取当前选中表
            selected_table = self.tables_list.get(self.tables_list.curselection())
            if not selected_table:
                messagebox.showwarning("警告", "请先选择要导出的表")
                return

            # 弹出保存文件对话框
            file_path = filedialog.asksaveasfilename(
                defaultextension='.xlsx',
                filetypes=[('Excel文件', '*.xlsx'), 
                          ('CSV文件', '*.csv'),
                          ('所有文件', '*.*')],
                title="保存导出文件"
            )
            if not file_path:
                return

            # 执行查询获取完整数据
            cursor = self.connection.cursor()
            cursor.execute(f"SELECT * FROM {selected_table}")
            rows = cursor.fetchall()
            columns = [desc[0] for desc in cursor.description]
            
            # 使用pandas处理数据导出
            df = pd.DataFrame(rows, columns=columns)
            
            if file_path.endswith('.xlsx'):
                df.to_excel(file_path, index=False)
            else:
                df.to_csv(file_path, index=False)
                
            messagebox.showinfo("成功", f"数据已导出到:\n{file_path}")
            
        except Error as e:
            messagebox.showerror("导出错误", f"导出失败: {e}")
        finally:
            if 'cursor' in locals():
                cursor.close()


if __name__ == "__main__":
    root = tk.Tk()
    app = DatabaseManager(root)
    root.mainloop()
```
