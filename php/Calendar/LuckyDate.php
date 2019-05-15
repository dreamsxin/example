<?php

class exportLuckyDate
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'export:luckydate';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = '导入择吉日期';

    /**
     * Create a new command instance.
     *
     * @return void
     */
    public function __construct()
    {
        parent::__construct();
    }

    /**
     * Execute the console command.
     *
     * @return mixed
     */
    public function handle()
    {
        $FortuneRelling = new FortuneTelling;
        $rs = $FortuneRelling->baiZiPaiPan(1,'1994-06-07','16:03:00');
        dd($rs);
        $calendar = new Calendar();

        for ($i=0; $i < 35580; $i++) { 
            $date = date('Y-m-d',strtotime("+$i day"));
            $date_fomat = explode('-', $date);

            $year = $date_fomat[0]; 
            $month = $date_fomat[1];
            $day = $date_fomat[2];

            // $result = $calendar->solar(2020,05,23); // 阳历
            $result = $calendar->solar($year, $month, $day); // 阳历
            $ganzhi_year = $result['ganzhi_year'];
            $ganzhi_month = $result['ganzhi_month'];
            $ganzhi_day = $result['ganzhi_day'];

            $gan_year = mb_substr($ganzhi_year,0,1);
            $zhi_year = mb_substr($ganzhi_year,-1,1);
            $gan_month = mb_substr($ganzhi_month,0,1);
            $zhi_month = mb_substr($ganzhi_month,-1,1);
            $gan_day = mb_substr($ganzhi_day,0,1);
            $zhi_day = mb_substr($ganzhi_day,-1,1);

            $lunar_month = str_replace('闰', '', $result['lunar_month_chinese']);
            $lunar_day = $result['lunar_day_chinese'];
            $jianchu = config("calendar.黑黄道日.$lunar_month.$zhi_day.jianchu");
            $xingxiu = config("calendar.星宿.$lunar_month.$lunar_day");

            $jianchu_yi = config("calendar.建除十二神.$jianchu.yi");
            $xingxiu_yi = config("calendar.星宿宜忌.$xingxiu.yi");
            $jianchu_ji = config("calendar.建除十二神.$jianchu.ji");
            $xingxiu_ji = config("calendar.星宿宜忌.$xingxiu.ji");
            $all_yi = array_unique(array_merge($jianchu_yi,$xingxiu_yi));
            $all_ji = array_unique(array_merge($jianchu_ji,$xingxiu_ji));

            $real_yi = array_diff($all_yi,  $all_ji);
            foreach ($all_yi as $key => $value) {
                $input = [
                    'date'=>$date,
                    'name'=>$value
                ];

                $LuckyDate = new LuckyDate;
                $LuckyDate->fill($input)->save();
            }
            
            echo $date.'
            ';
        }

        // $result = $calendar->lunar(2017, 4, 10); // 阴历
        // $result = $calendar->solar(2017, 5, 5, 23)
    }
}
