
```typescript
<template>
  <div class="app-container detail">

    <el-tabs type="card" v-model="activeName" @tab-click="handleClick">
      <el-tab-pane label="今日" name="today">
        <canvas id="todayChart"></canvas>
      </el-tab-pane>
      <el-tab-pane label="昨日" name="hour">
        <canvas id="hourChart"></canvas></el-tab-pane>
      <el-tab-pane label="按天查询" name="day">
        
      <el-row>
        <el-col :lg="24" :xs="24">
          <div class="search">
            <el-form ref="queryFormRef" :model="queryParams" :inline="true">

              <el-form-item label="日期" prop="created_at">
                <el-date-picker v-model="queryParams.created_at" type="daterange" start-placeholder="开始日期"
                  end-placeholder="结束日期" format="YYYY-MM-DD" value-format="YYYY-MM-DD" @change="handleQuery">
                </el-date-picker>
              </el-form-item>

              <el-form-item>
                <el-button type="primary" @click="handleQuery"><i-ep-search />搜索</el-button>
                <el-button @click="resetQuery"><i-ep-refresh />重置</el-button>
              </el-form-item>
            </el-form>
          </div>
        </el-col>
      </el-row>
        <canvas id="dayChart"></canvas></el-tab-pane>
    </el-tabs>

  </div>
</template>

<script setup lang="ts">
import {
  getUserOnlineApi
} from '@/api/user';
import { ref, reactive, onMounted } from 'vue';
import { ElForm } from 'element-plus';
import Chart from 'chart.js/auto'

var activeName = ref("today");
const queryFormRef = ref(ElForm); // 查询表单
var charts: Map<string, any>;

/**
 * 查询参数
 */
const queryParams = reactive<{ [key: string]: any }>({
  startDate: '',
  endDate: '',
});

interface OnlineItem {
  label?: string;
  num?: number;
}

/**
 * 获取列表数据
 */
const getUserOnlineList = async (name: string) => {

  const params = {
    ...queryParams
  };
  // created_at
  console.log(params);
  if (queryParams.created_at && queryParams.created_at.length) {
    params.startDate = queryParams.created_at[0];
    params.endDate = queryParams.created_at[1];
  }

  getUserOnlineApi(name, params)
    .then(({ data, code, msg }) => {
      if (code !== 200) {
        return ElMessage.error(msg);
      }

      console.log(data);

      charts.get(name).config.data.labels = data.map((row: OnlineItem) => row.label).reverse();
      charts.get(name).config.data.datasets.forEach(function (dataset: any) {
        dataset.data = data.map((row: OnlineItem) => row.num).reverse();
      });
      charts.get(name).update();
    })
    .finally(() => {
    });
};

const handleClick = function (tab: any, event: any) {
  console.log("handleClick", tab.paneName, event);
  getUserOnlineList(tab.paneName);
};

onMounted(() => {
  charts = new Map<string, Chart>();
  var ctx = document.getElementById("todayChart") as HTMLCanvasElement;
  charts.set("today", new Chart(
    ctx,
    {
      type: 'line',
      data: {
        labels: [],
        datasets: [
          {
            label: '今日在线用户数',
            data: []
          }
        ]
      }
    }
  ));
  ctx = document.getElementById("hourChart") as HTMLCanvasElement;
  charts.set("hour", new Chart(
    ctx,
    {
      type: 'line',
      data: {
        labels: [],
        datasets: [
          {
            label: '昨日在线用户数',
            data: []
          }
        ]
      }
    }
  ));
  ctx = document.getElementById("dayChart") as HTMLCanvasElement;
  charts.set("day", new Chart(
    ctx,
    {
      type: 'line',
      data: {
        labels: [],
        datasets: [
          {
            label: '最近30天',
            data: []
          }
        ]
      }
    }
  ));
  getUserOnlineList(activeName.value);
});

/**
 * 查询
 */
 function handleQuery() {
  getUserOnlineList(activeName.value);
}

/**
 * 重置查询
 */
 function resetQuery() {
  queryFormRef.value.resetFields();
  handleQuery();
}
</script>
```
