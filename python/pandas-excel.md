```python
import pandas as pd
from sqlalchemy import create_engine
from contextlib import contextmanager

# 数据源配置字典
DATABASES = {
    'test': 'mysql+pymysql://test:test@localhost:3306/test',
}

@contextmanager
def switch_database(db_name):
    engine = create_engine(DATABASES[db_name], connect_args={'charset': 'utf8mb4'})
    try:
        yield engine.connect()
    finally:
        engine.dispose()

# 使用示例
with switch_database('test') as conn:
    # 场景1：分块读取SQL并写入CSV（适合超大数据导出）
    query = r"SELECT xxxx"

    chunk_size = 50000  # 每块5万条记录
    chunks = []  # 存储所有数据块
    
    print("开始读取数据...")
    total_rows = 0
    
    # 分块读取所有数据
    for chunk_idx, chunk in enumerate(pd.read_sql_query(query, conn, chunksize=chunk_size)):
        print(f"读取第 {chunk_idx + 1} 块，共 {len(chunk)} 条记录")
        chunks.append(chunk)
        total_rows += len(chunk)
    
    print(f"数据读取完成，共 {total_rows} 条原始记录")
    
    # 合并所有数据块
    if chunks:
        df_all = pd.concat(chunks, ignore_index=True)
        print(f"合并后总数据量：{len(df_all)} 条记录")
        
        # 按照 company_id 合并去重
        if '企业ID' in df_all.columns and df_all['企业ID'].notna().any():
            print("开始按企业ID合并去重...")
            
            # 定义聚合函数：时间字段取最大值，其他取第一个
            agg_dict = {}
            for col in df_all.columns:
                if col in ['套餐到期时间', '代理到期时间']:
                    agg_dict[col] = 'max'
                elif col != '企业ID':  # 分组键不需要聚合
                    agg_dict[col] = 'first'
            
            # 按企业ID分组并聚合
            df_merged = df_all.groupby('企业ID', as_index=False).agg(agg_dict)
            
            print(f"去重后数据量：{len(df_merged)} 条记录")
            print(f"去重率：{1 - len(df_merged)/len(df_all):.2%}")
        else:
            df_merged = df_all
            print("企业ID字段为空，跳过合并去重步骤")
        
        # 导出到Excel
        print("开始写入Excel文件...")
        with pd.ExcelWriter('test.xlsx', engine='xlsxwriter') as writer:
            df_merged.to_excel(writer, sheet_name='订单用户数据', index=False)
            
            # 自动调整列宽（可选）
            worksheet = writer.sheets['订单用户数据']
            for i, col in enumerate(df_merged.columns):
                column_width = max(df_merged[col].astype(str).map(len).max(), len(col)) + 2
                worksheet.set_column(i, i, min(column_width, 50))
        
        print(f"数据已导出到 'test.xlsx'，共 {len(df_merged)} 条记录")
    else:
        print("没有读取到数据")
```
