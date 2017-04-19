<?php

/**
 * <sql>
 * SET search_path = yutong;
 * SET default_tablespace = '';
 * SET default_with_oids = false;
 *
 * DROP TABLE IF EXISTS countries CASCADE;
 * CREATE TABLE countries (
 * 	id serial,
 * 	name character varying(30),					-- 国家名称
 * 	code character varying(10),					-- 字母代号
 * 	num character varying(10),					-- 数字代号
 *
 * 	CONSTRAINT countries_name_unique UNIQUE (name),
 * 	CONSTRAINT countries_code_unique UNIQUE (code),
 * 	CONSTRAINT countries_num_unique UNIQUE (num),
 * 	CONSTRAINT countries_id_pkey PRIMARY KEY (id)
 * );
 * <sql>
 **/
 
$html = <<<HTML
<table style="border-collapse: collapse" width="600" cellspacing="0" cellpadding="0" border="1">
<tbody>
<tr>
<td width="30%" height="18">Angola</td>
<td width="21%" align="center" height="18">安哥拉</td>
<td width="16%" align="center" height="18">AO</td>
<td width="16%" align="center" height="18">244</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Afghanistan</td>
<td width="21%" align="center" height="18">阿富汗</td>
<td width="16%" align="center" height="18">AF</td>
<td width="16%" align="center" height="18">93</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Albania</td>
<td width="21%" align="center" height="18">阿尔巴尼亚</td>
<td width="16%" align="center" height="18">AL</td>
<td width="16%" align="center" height="18">355</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Algeria</td>
<td width="21%" align="center" height="18">阿尔及利亚</td>
<td width="16%" align="center" height="18">DZ</td>
<td width="16%" align="center" height="18">213</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Andorra</td>
<td width="21%" align="center" height="18">安道尔共和国</td>
<td width="16%" align="center" height="18">AD</td>
<td width="16%" align="center" height="18">376</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Anguilla</td>
<td width="21%" align="center" height="18">安圭拉岛</td>
<td width="16%" align="center" height="18">AI</td>
<td width="16%" align="center" height="18">1264</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Antigua and Barbuda</td>
<td width="21%" align="center" height="18">安提瓜和巴布达</td>
<td width="16%" align="center" height="18">AG</td>
<td width="16%" align="center" height="18">1268</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Argentina</td>
<td width="21%" align="center" height="18">阿根廷</td>
<td width="16%" align="center" height="18">AR</td>
<td width="16%" align="center" height="18">54</td>
<td width="17%" align="center" height="18">-11</td>
</tr>
<tr>
<td width="30%" height="18">Armenia</td>
<td width="21%" align="center" height="18">亚美尼亚</td>
<td width="16%" align="center" height="18">AM</td>
<td width="16%" align="center" height="18">374</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Ascension</td>
<td width="21%" align="center" height="18">阿森松</td>
<td width="16%" align="center" height="18">AC</td>
<td width="16%" align="center" height="18">247</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Australia</td>
<td width="21%" align="center" height="18">澳大利亚</td>
<td width="16%" align="center" height="18">AU</td>
<td width="16%" align="center" height="18">61</td>
<td width="17%" align="center" height="18">+2</td>
</tr>
<tr>
<td width="30%" height="18">Austria</td>
<td width="21%" align="center" height="18">奥地利</td>
<td width="16%" align="center" height="18">AT</td>
<td width="16%" align="center" height="18">43</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Azerbaijan</td>
<td width="21%" align="center" height="18">阿塞拜疆</td>
<td width="16%" align="center" height="18">AZ</td>
<td width="16%" align="center" height="18">994</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Bahamas</td>
<td width="21%" align="center" height="18">巴哈马</td>
<td width="16%" align="center" height="18">BS</td>
<td width="16%" align="center" height="18">1242</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Bahrain</td>
<td width="21%" align="center" height="18">巴林</td>
<td width="16%" align="center" height="18">BH</td>
<td width="16%" align="center" height="18">973</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Bangladesh</td>
<td width="21%" align="center" height="18">孟加拉国</td>
<td width="16%" align="center" height="18">BD</td>
<td width="16%" align="center" height="18">880</td>
<td width="17%" align="center" height="18">-2</td>
</tr>
<tr>
<td width="30%" height="18">Barbados</td>
<td width="21%" align="center" height="18">巴巴多斯</td>
<td width="16%" align="center" height="18">BB</td>
<td width="16%" align="center" height="18">1246</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Belarus</td>
<td width="21%" align="center" height="18">白俄罗斯</td>
<td width="16%" align="center" height="18">BY</td>
<td width="16%" align="center" height="18">375</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Belgium</td>
<td width="21%" align="center" height="18">比利时</td>
<td width="16%" align="center" height="18">BE</td>
<td width="16%" align="center" height="18">32</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Belize</td>
<td width="21%" align="center" height="18">伯利兹</td>
<td width="16%" align="center" height="18">BZ</td>
<td width="16%" align="center" height="18">501</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Benin</td>
<td width="21%" align="center" height="18">贝宁</td>
<td width="16%" align="center" height="18">BJ</td>
<td width="16%" align="center" height="18">229</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Bermuda Is.</td>
<td width="21%" align="center" height="18">百慕大群岛</td>
<td width="16%" align="center" height="18">BM</td>
<td width="16%" align="center" height="18">1441</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Bolivia</td>
<td width="21%" align="center" height="18">玻利维亚</td>
<td width="16%" align="center" height="18">BO</td>
<td width="16%" align="center" height="18">591</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Botswana</td>
<td width="21%" align="center" height="18">博茨瓦纳</td>
<td width="16%" align="center" height="18">BW</td>
<td width="16%" align="center" height="18">267</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Brazil</td>
<td width="21%" align="center" height="18">巴西</td>
<td width="16%" align="center" height="18">BR</td>
<td width="16%" align="center" height="18">55</td>
<td width="17%" align="center" height="18">-11</td>
</tr>
<tr>
<td width="30%" height="18">Brunei</td>
<td width="21%" align="center" height="18">文莱</td>
<td width="16%" align="center" height="18">BN</td>
<td width="16%" align="center" height="18">673</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Bulgaria</td>
<td width="21%" align="center" height="18">保加利亚</td>
<td width="16%" align="center" height="18">BG</td>
<td width="16%" align="center" height="18">359</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Burkina-faso</td>
<td width="21%" align="center" height="18">布基纳法索</td>
<td width="16%" align="center" height="18">BF</td>
<td width="16%" align="center" height="18">226</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Burma</td>
<td width="21%" align="center" height="18">缅甸</td>
<td width="16%" align="center" height="18">MM</td>
<td width="16%" align="center" height="18">95</td>
<td width="17%" align="center" height="18">-1.3</td>
</tr>
<tr>
<td width="30%" height="18">Burundi</td>
<td width="21%" align="center" height="18">布隆迪</td>
<td width="16%" align="center" height="18">BI</td>
<td width="16%" align="center" height="18">257</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Cameroon</td>
<td width="21%" align="center" height="18">喀麦隆</td>
<td width="16%" align="center" height="18">CM</td>
<td width="16%" align="center" height="18">237</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Canada</td>
<td width="21%" align="center" height="18">加拿大</td>
<td width="16%" align="center" height="18">CA</td>
<td width="16%" align="center" height="18">1</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Cayman Is.</font></td>
<td width="21%" align="center" height="18">开曼群岛</td>
<td width="16%" align="center" height="18">KY</td>
<td width="16%" align="center" height="18">1345</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Central African Republic</td>
<td width="21%" align="center" height="18">中非共和国</td>
<td width="16%" align="center" height="18">CF</td>
<td width="16%" align="center" height="18">236</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Chad</td>
<td width="21%" align="center" height="18">乍得</td>
<td width="16%" align="center" height="18">TD</td>
<td width="16%" align="center" height="18">235</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Chile</td>
<td width="21%" align="center" height="18">智利</td>
<td width="16%" align="center" height="18">CL</td>
<td width="16%" align="center" height="18">56</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">China</td>
<td width="21%" align="center" height="18">中国</td>
<td width="16%" align="center" height="18">CN</td>
<td width="16%" align="center" height="18">86</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Colombia</td>
<td width="21%" align="center" height="18">哥伦比亚</td>
<td width="16%" align="center" height="18">CO</td>
<td width="16%" align="center" height="18">57</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Congo</td>
<td width="21%" align="center" height="18">刚果</td>
<td width="16%" align="center" height="18">CG</td>
<td width="16%" align="center" height="18">242</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Cook Is.</font></td>
<td width="21%" align="center" height="18">库克群岛</td>
<td width="16%" align="center" height="18">CK</td>
<td width="16%" align="center" height="18">682</td>
<td width="17%" align="center" height="18">-18.3</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Costa Rica</font></td>
<td width="21%" align="center" height="18">哥斯达黎加</td>
<td width="16%" align="center" height="18">CR</td>
<td width="16%" align="center" height="18">506</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Cuba</td>
<td width="21%" align="center" height="18">古巴</td>
<td width="16%" align="center" height="18">CU</td>
<td width="16%" align="center" height="18">53</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Cyprus</td>
<td width="21%" align="center" height="18">塞浦路斯</td>
<td width="16%" align="center" height="18">CY</td>
<td width="16%" align="center" height="18">357</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Czech Republic </td>
<td width="21%" align="center" height="18">捷克</td>
<td width="16%" align="center" height="18">CZ</td>
<td width="16%" align="center" height="18">420</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Denmark</td>
<td width="21%" align="center" height="18">丹麦</td>
<td width="16%" align="center" height="18">DK</td>
<td width="16%" align="center" height="18">45</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Djibouti</td>
<td width="21%" align="center" height="18">吉布提</td>
<td width="16%" align="center" height="18">DJ</td>
<td width="16%" align="center" height="18">253</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Dominica Rep.</td>
<td width="21%" align="center" height="18">多米尼加共和国</td>
<td width="16%" align="center" height="18">DO</td>
<td width="16%" align="center" height="18">1890</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Ecuador</td>
<td width="21%" align="center" height="18">厄瓜多尔</td>
<td width="16%" align="center" height="18">EC</td>
<td width="16%" align="center" height="18">593</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Egypt</td>
<td width="21%" align="center" height="18">埃及</td>
<td width="16%" align="center" height="18">EG</td>
<td width="16%" align="center" height="18">20</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">EI Salvador</font></td>
<td width="21%" align="center" height="18">萨尔瓦多</td>
<td width="16%" align="center" height="18">SV</td>
<td width="16%" align="center" height="18">503</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Estonia</td>
<td width="21%" align="center" height="18">爱沙尼亚</td>
<td width="16%" align="center" height="18">EE</td>
<td width="16%" align="center" height="18">372</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Ethiopia</td>
<td width="21%" align="center" height="18">埃塞俄比亚</td>
<td width="16%" align="center" height="18">ET</td>
<td width="16%" align="center" height="18">251</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Fiji</td>
<td width="21%" align="center" height="18">斐济</td>
<td width="16%" align="center" height="18">FJ</td>
<td width="16%" align="center" height="18">679</td>
<td width="17%" align="center" height="18">+4</td>
</tr>
<tr>
<td width="30%" height="18">Finland</td>
<td width="21%" align="center" height="18">芬兰</td>
<td width="16%" align="center" height="18">FI</td>
<td width="16%" align="center" height="18">358</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="25">France</td>
<td width="21%" align="center" height="25">法国</td>
<td width="16%" align="center" height="25">FR</td>
<td width="16%" align="center" height="25">33</td>
<td width="17%" align="center" height="25">-8</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">French Guiana</font></td>
<td width="21%" align="center" height="18">法属圭亚那</td>
<td width="16%" align="center" height="18">GF</td>
<td width="16%" align="center" height="18">594</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Gabon</td>
<td width="21%" align="center" height="18">加蓬</td>
<td width="16%" align="center" height="18">GA</td>
<td width="16%" align="center" height="18">241</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Gambia</td>
<td width="21%" align="center" height="18">冈比亚</td>
<td width="16%" align="center" height="18">GM</td>
<td width="16%" align="center" height="18">220</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Georgia </td>
<td width="21%" align="center" height="18">格鲁吉亚</td>
<td width="16%" align="center" height="18">GE</td>
<td width="16%" align="center" height="18">995</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Germany </td>
<td width="21%" align="center" height="18">德国</td>
<td width="16%" align="center" height="18">DE</td>
<td width="16%" align="center" height="18">49</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Ghana</td>
<td width="21%" align="center" height="18">加纳</td>
<td width="16%" align="center" height="18">GH</td>
<td width="16%" align="center" height="18">233</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Gibraltar</td>
<td width="21%" align="center" height="18">直布罗陀</td>
<td width="16%" align="center" height="18">GI</td>
<td width="16%" align="center" height="18">350</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Greece</td>
<td width="21%" align="center" height="18">希腊</td>
<td width="16%" align="center" height="18">GR</td>
<td width="16%" align="center" height="18">30</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Grenada</td>
<td width="21%" align="center" height="18">格林纳达</td>
<td width="16%" align="center" height="18">GD</td>
<td width="16%" align="center" height="18">1473</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Guam</td>
<td width="21%" align="center" height="18">关岛</td>
<td width="16%" align="center" height="18">GU</td>
<td width="16%" align="center" height="18">1671</td>
<td width="17%" align="center" height="18">+2</td>
</tr>
<tr>
<td width="30%" height="18">Guatemala</td>
<td width="21%" align="center" height="18">危地马拉</td>
<td width="16%" align="center" height="18">GT</td>
<td width="16%" align="center" height="18">502</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Guinea</td>
<td width="21%" align="center" height="18">几内亚</td>
<td width="16%" align="center" height="18">GN</td>
<td width="16%" align="center" height="18">224</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Guyana</td>
<td width="21%" align="center" height="18">圭亚那</td>
<td width="16%" align="center" height="18">GY</td>
<td width="16%" align="center" height="18">592</td>
<td width="17%" align="center" height="18">-11</td>
</tr>
<tr>
<td width="30%" height="18">Haiti</td>
<td width="21%" align="center" height="18">海地</td>
<td width="16%" align="center" height="18">HT</td>
<td width="16%" align="center" height="18">509</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Honduras</td>
<td width="21%" align="center" height="18">洪都拉斯</td>
<td width="16%" align="center" height="18">HN</td>
<td width="16%" align="center" height="18">504</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Hongkong</td>
<td width="21%" align="center" height="18">香港</td>
<td width="16%" align="center" height="18">HK</td>
<td width="16%" align="center" height="18">852</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Hungary</td>
<td width="21%" align="center" height="18">匈牙利</td>
<td width="16%" align="center" height="18">HU</td>
<td width="16%" align="center" height="18">36</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Iceland</td>
<td width="21%" align="center" height="18">冰岛</td>
<td width="16%" align="center" height="18">IS</td>
<td width="16%" align="center" height="18">354</td>
<td width="17%" align="center" height="18">-9</td>
</tr>
<tr>
<td width="30%" height="18">India</td>
<td width="21%" align="center" height="18">印度</td>
<td width="16%" align="center" height="18">IN</td>
<td width="16%" align="center" height="18">91</td>
<td width="17%" align="center" height="18">-2.3</td>
</tr>
<tr>
<td width="30%" height="18">Indonesia</td>
<td width="21%" align="center" height="18">印度尼西亚</td>
<td width="16%" align="center" height="18">ID</td>
<td width="16%" align="center" height="18">62</td>
<td width="17%" align="center" height="18">-0.3</td>
</tr>
<tr>
<td width="30%" height="18">Iran</td>
<td width="21%" align="center" height="18">伊朗</td>
<td width="16%" align="center" height="18">IR</td>
<td width="16%" align="center" height="18">98</td>
<td width="17%" align="center" height="18">-4.3</td>
</tr>
<tr>
<td width="30%" height="18">Iraq</td>
<td width="21%" align="center" height="18">伊拉克</td>
<td width="16%" align="center" height="18">IQ</td>
<td width="16%" align="center" height="18">964</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Ireland</td>
<td width="21%" align="center" height="18">爱尔兰</td>
<td width="16%" align="center" height="18">IE</td>
<td width="16%" align="center" height="18">353</td>
<td width="17%" align="center" height="18">-4.3</td>
</tr>
<tr>
<td width="30%" height="18">Israel</td>
<td width="21%" align="center" height="18">以色列</td>
<td width="16%" align="center" height="18">IL</td>
<td width="16%" align="center" height="18">972</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Italy</td>
<td width="21%" align="center" height="18">意大利</td>
<td width="16%" align="center" height="18">IT</td>
<td width="16%" align="center" height="18">39</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Ivory Coast</font></td>
<td width="21%" align="center" height="18">科特迪瓦</td>
<td width="16%" align="center" height="18">CIV</td>
<td width="16%" align="center" height="18">225</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Jamaica</td>
<td width="21%" align="center" height="18">牙买加</td>
<td width="16%" align="center" height="18">JM</td>
<td width="16%" align="center" height="18">1876</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Japan</td>
<td width="21%" align="center" height="18">日本</td>
<td width="16%" align="center" height="18">JP</td>
<td width="16%" align="center" height="18">81</td>
<td width="17%" align="center" height="18">+1</td>
</tr>
<tr>
<td width="30%" height="18">Jordan</td>
<td width="21%" align="center" height="18">约旦</td>
<td width="16%" align="center" height="18">JO</td>
<td width="16%" align="center" height="18">962</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Kampuchea (Cambodia )</td>
<td width="21%" align="center" height="18">柬埔寨</td>
<td width="16%" align="center" height="18">KH</td>
<td width="16%" align="center" height="18">855</td>
<td width="17%" align="center" height="18">-1</td>
</tr>
<tr>
<td width="30%" height="18">Kazakstan</td>
<td width="21%" align="center" height="18">哈萨克斯坦</td>
<td width="16%" align="center" height="18">KZ</td>
<td width="16%" align="center" height="18">327</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Kenya</td>
<td width="21%" align="center" height="18">肯尼亚</td>
<td width="16%" align="center" height="18">KE</td>
<td width="16%" align="center" height="18">254</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Korea</td>
<td width="21%" align="center" height="18">韩国</td>
<td width="16%" align="center" height="18">KR</td>
<td width="16%" align="center" height="18">82</td>
<td width="17%" align="center" height="18">+1</td>
</tr>
<tr>
<td width="30%" height="18">Kuwait</td>
<td width="21%" align="center" height="18">科威特</td>
<td width="16%" align="center" height="18">KW</td>
<td width="16%" align="center" height="18">965</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Kyrgyzstan </td>
<td width="21%" align="center" height="18">吉尔吉斯坦</td>
<td width="16%" align="center" height="18">KG</td>
<td width="16%" align="center" height="18">331</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Laos</td>
<td width="21%" align="center" height="18">老挝</td>
<td width="16%" align="center" height="18">LA</td>
<td width="16%" align="center" height="18">856</td>
<td width="17%" align="center" height="18">-1</td>
</tr>
<tr>
<td width="30%" height="18">Latvia </td>
<td width="21%" align="center" height="18">拉脱维亚</td>
<td width="16%" align="center" height="18">LV</td>
<td width="16%" align="center" height="18">371</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Lebanon</td>
<td width="21%" align="center" height="18">黎巴嫩</td>
<td width="16%" align="center" height="18">LB</td>
<td width="16%" align="center" height="18">961</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Lesotho</td>
<td width="21%" align="center" height="18">莱索托</td>
<td width="16%" align="center" height="18">LS</td>
<td width="16%" align="center" height="18">266</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Liberia</td>
<td width="21%" align="center" height="18">利比里亚</td>
<td width="16%" align="center" height="18">LR</td>
<td width="16%" align="center" height="18">231</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Libya</td>
<td width="21%" align="center" height="18">利比亚</td>
<td width="16%" align="center" height="18">LY</td>
<td width="16%" align="center" height="18">218</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Liechtenstein</td>
<td width="21%" align="center" height="18">列支敦士登</td>
<td width="16%" align="center" height="18">LI</td>
<td width="16%" align="center" height="18">423</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Lithuania</td>
<td width="21%" align="center" height="18">立陶宛</td>
<td width="16%" align="center" height="18">LT</td>
<td width="16%" align="center" height="18">370</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Luxembourg</td>
<td width="21%" align="center" height="18">卢森堡</td>
<td width="16%" align="center" height="18">LU</td>
<td width="16%" align="center" height="18">352</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Macao</td>
<td width="21%" align="center" height="18">澳门</td>
<td width="16%" align="center" height="18">MO</td>
<td width="16%" align="center" height="18">853</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Madagascar</td>
<td width="21%" align="center" height="18">马达加斯加</td>
<td width="16%" align="center" height="18">MG</td>
<td width="16%" align="center" height="18">261</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Malawi</td>
<td width="21%" align="center" height="18">马拉维</td>
<td width="16%" align="center" height="18">MW</td>
<td width="16%" align="center" height="18">265</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Malaysia</td>
<td width="21%" align="center" height="18">马来西亚</td>
<td width="16%" align="center" height="18">MY</td>
<td width="16%" align="center" height="18">60</td>
<td width="17%" align="center" height="18">-0.5</td>
</tr>
<tr>
<td width="30%" height="18">Maldives</td>
<td width="21%" align="center" height="18">马尔代夫</td>
<td width="16%" align="center" height="18">MV</td>
<td width="16%" align="center" height="18">960</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Mali</td>
<td width="21%" align="center" height="18">马里</td>
<td width="16%" align="center" height="18">ML</td>
<td width="16%" align="center" height="18">223</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Malta</td>
<td width="21%" align="center" height="18">马耳他</td>
<td width="16%" align="center" height="18">MT</td>
<td width="16%" align="center" height="18">356</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Mariana Is</td>
<td width="21%" align="center" height="18">马里亚纳群岛</td>
<td width="16%" align="center" height="18">MP</td>
<td width="16%" align="center" height="18">1670</td>
<td width="17%" align="center" height="18">+1</td>
</tr>
<tr>
<td width="30%" height="18">Martinique</td>
<td width="21%" align="center" height="18">马提尼克</td>
<td width="16%" align="center" height="18">MQ</td>
<td width="16%" align="center" height="18">596</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Mauritius</td>
<td width="21%" align="center" height="18">毛里求斯</td>
<td width="16%" align="center" height="18">MU</td>
<td width="16%" align="center" height="18">230</td>
<td width="17%" align="center" height="18">-4</td>
</tr>
<tr>
<td width="30%" height="18">Mexico</td>
<td width="21%" align="center" height="18">墨西哥</td>
<td width="16%" align="center" height="18">MX</td>
<td width="16%" align="center" height="18">52</td>
<td width="17%" align="center" height="18">-15</td>
</tr>
<tr>
<td width="30%" height="18">Moldova Republic of </td>
<td width="21%" align="center" height="18">摩尔多瓦</td>
<td width="16%" align="center" height="18">MD</td>
<td width="16%" align="center" height="18">373</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Monaco</td>
<td width="21%" align="center" height="18">摩纳哥</td>
<td width="16%" align="center" height="18">MC</td>
<td width="16%" align="center" height="18">377</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Mongolia </td>
<td width="21%" align="center" height="18">蒙古</td>
<td width="16%" align="center" height="18">MN</td>
<td width="16%" align="center" height="18">976</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Montserrat Is</td>
<td width="21%" align="center" height="18">蒙特塞拉特岛</td>
<td width="16%" align="center" height="18">MS</td>
<td width="16%" align="center" height="18">1664</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Morocco</td>
<td width="21%" align="center" height="18">摩洛哥</td>
<td width="16%" align="center" height="18">MA</td>
<td width="16%" align="center" height="18">212</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Mozambique</td>
<td width="21%" align="center" height="18">莫桑比克</td>
<td width="16%" align="center" height="18">MZ</td>
<td width="16%" align="center" height="18">258</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Namibia </td>
<td width="21%" align="center" height="18">纳米比亚</td>
<td width="16%" align="center" height="18">NA</td>
<td width="16%" align="center" height="18">264</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Nauru</td>
<td width="21%" align="center" height="18">瑙鲁</td>
<td width="16%" align="center" height="18">NR</td>
<td width="16%" align="center" height="18">674</td>
<td width="17%" align="center" height="18">+4</td>
</tr>
<tr>
<td width="30%" height="18">Nepal</td>
<td width="21%" align="center" height="18">尼泊尔</td>
<td width="16%" align="center" height="18">NP</td>
<td width="16%" align="center" height="18">977</td>
<td width="17%" align="center" height="18">-2.3</td>
</tr>
<tr>
<td width="30%" height="18">Netheriands Antilles</td>
<td width="21%" align="center" height="18">荷属安的列斯</td>
<td width="16%" align="center" height="18">AHO</td>
<td width="16%" align="center" height="18">599</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Netherlands</td>
<td width="21%" align="center" height="18">荷兰</td>
<td width="16%" align="center" height="18">NL</td>
<td width="16%" align="center" height="18">31</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">New Zealand</font></td>
<td width="21%" align="center" height="18">新西兰</td>
<td width="16%" align="center" height="18">NZ</td>
<td width="16%" align="center" height="18">64</td>
<td width="17%" align="center" height="18">+4</td>
</tr>
<tr>
<td width="30%" height="18">Nicaragua</td>
<td width="21%" align="center" height="18">尼加拉瓜</td>
<td width="16%" align="center" height="18">NI</td>
<td width="16%" align="center" height="18">505</td>
<td width="17%" align="center" height="18">-14</td>
</tr>
<tr>
<td width="30%" height="18">Niger</td>
<td width="21%" align="center" height="18">尼日尔</td>
<td width="16%" align="center" height="18"><font face="宋体">NE</font></td>
<td width="16%" align="center" height="18">227</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Nigeria</td>
<td width="21%" align="center" height="18">尼日利亚</td>
<td width="16%" align="center" height="18">NG</td>
<td width="16%" align="center" height="18">234</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">North Korea</td>
<td width="21%" align="center" height="18">朝鲜</td>
<td width="16%" align="center" height="18">KP</td>
<td width="16%" align="center" height="18">850</td>
<td width="17%" align="center" height="18">+1</td>
</tr>
<tr>
<td width="30%" height="18">Norway</td>
<td width="21%" align="center" height="18">挪威</td>
<td width="16%" align="center" height="18">NO</td>
<td width="16%" align="center" height="18">47</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Oman</td>
<td width="21%" align="center" height="18">阿曼</td>
<td width="16%" align="center" height="18">OM</td>
<td width="16%" align="center" height="18">968</td>
<td width="17%" align="center" height="18">-4</td>
</tr>
<tr>
<td width="30%" height="18">Pakistan</td>
<td width="21%" align="center" height="18">巴基斯坦</td>
<td width="16%" align="center" height="18">PK</td>
<td width="16%" align="center" height="18">92</td>
<td width="17%" align="center" height="18">-2.3</td>
</tr>
<tr>
<td width="30%" height="18">Panama</td>
<td width="21%" align="center" height="18">巴拿马</td>
<td width="16%" align="center" height="18">PA</td>
<td width="16%" align="center" height="18">507</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Papua New
Cuinea</font></td>
<td width="21%" align="center" height="18">巴布亚新几内亚</td>
<td width="16%" align="center" height="18">PG</td>
<td width="16%" align="center" height="18">675</td>
<td width="17%" align="center" height="18">+2</td>
</tr>
<tr>
<td width="30%" height="18">Paraguay</td>
<td width="21%" align="center" height="18">巴拉圭</td>
<td width="16%" align="center" height="18">PY</td>
<td width="16%" align="center" height="18">595</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Peru</td>
<td width="21%" align="center" height="18">秘鲁</td>
<td width="16%" align="center" height="18">PE</td>
<td width="16%" align="center" height="18">51</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Philippines</td>
<td width="21%" align="center" height="18">菲律宾</td>
<td width="16%" align="center" height="18">PH</td>
<td width="16%" align="center" height="18">63</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Poland</td>
<td width="21%" align="center" height="18">波兰</td>
<td width="16%" align="center" height="18">PL</td>
<td width="16%" align="center" height="18">48</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">French Polynesia</font></td>
<td width="21%" align="center" height="18">法属玻利尼西亚</td>
<td width="16%" align="center" height="18">PF</td>
<td width="16%" align="center" height="18">689</td>
<td width="17%" align="center" height="18">+3</td>
</tr>
<tr>
<td width="30%" height="18">Portugal</td>
<td width="21%" align="center" height="18">葡萄牙</td>
<td width="16%" align="center" height="18">PT</td>
<td width="16%" align="center" height="18">351</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Puerto Rico</font></td>
<td width="21%" align="center" height="18">波多黎各</td>
<td width="16%" align="center" height="18">PR</td>
<td width="16%" align="center" height="18">1787</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Qatar</td>
<td width="21%" align="center" height="18">卡塔尔</td>
<td width="16%" align="center" height="18">QA</td>
<td width="16%" align="center" height="18">974</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Reunion</td>
<td width="21%" align="center" height="18">留尼汪</td>
<td width="16%" align="center" height="18">RE</td>
<td width="16%" align="center" height="18">262</td>
<td width="17%" align="center" height="18">-4</td>
</tr>
<tr>
<td width="30%" height="18">Romania</td>
<td width="21%" align="center" height="18">罗马尼亚</td>
<td width="16%" align="center" height="18">RO</td>
<td width="16%" align="center" height="18">40</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Russia</td>
<td width="21%" align="center" height="18">俄罗斯</td>
<td width="16%" align="center" height="18">RU</td>
<td width="16%" align="center" height="18">7</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Saint Lueia</td>
<td width="21%" align="center" height="18">圣卢西亚</td>
<td width="16%" align="center" height="18">LC</td>
<td width="16%" align="center" height="18">1758</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Saint Vincent</td>
<td width="21%" align="center" height="18">圣文森特岛</td>
<td width="16%" align="center" height="18">VC</td>
<td width="16%" align="center" height="18">1784</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Samoa Eastern</font></td>
<td width="21%" align="center" height="18">东萨摩亚(美)</td>
<td width="16%" align="center" height="18">AS</td>
<td width="16%" align="center" height="18">684</td>
<td width="17%" align="center" height="18">-19</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Samoa Western</font></td>
<td width="21%" align="center" height="18">西萨摩亚</td>
<td width="16%" align="center" height="18">EH</td>
<td width="16%" align="center" height="18">685</td>
<td width="17%" align="center" height="18">-19</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">San Marino</font></td>
<td width="21%" align="center" height="18">圣马力诺</td>
<td width="16%" align="center" height="18">SM</td>
<td width="16%" align="center" height="18">378</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Sao Tome
and Principe</font></td>
<td width="21%" align="center" height="18">圣多美和普林西比</td>
<td width="16%" align="center" height="18">ST</td>
<td width="16%" align="center" height="18">239</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Saudi Arabia</font></td>
<td width="21%" align="center" height="18">沙特阿拉伯</td>
<td width="16%" align="center" height="18">SA</td>
<td width="16%" align="center" height="18">966</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Senegal</td>
<td width="21%" align="center" height="18">塞内加尔</td>
<td width="16%" align="center" height="18">SN</td>
<td width="16%" align="center" height="18">221</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Seychelles</td>
<td width="21%" align="center" height="18">塞舌尔</td>
<td width="16%" align="center" height="18">SC</td>
<td width="16%" align="center" height="18">248</td>
<td width="17%" align="center" height="18">-4</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Sierra Leone</font></td>
<td width="21%" align="center" height="18">塞拉利昂</td>
<td width="16%" align="center" height="18">SL</td>
<td width="16%" align="center" height="18">232</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Singapore</td>
<td width="21%" align="center" height="18">新加坡</td>
<td width="16%" align="center" height="18">SG</td>
<td width="16%" align="center" height="18">65</td>
<td width="17%" align="center" height="18">+0.3</td>
</tr>
<tr>
<td width="30%" height="18">Slovakia</td>
<td width="21%" align="center" height="18">斯洛伐克</td>
<td width="16%" align="center" height="18">SK</td>
<td width="16%" align="center" height="18">421</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Slovenia</td>
<td width="21%" align="center" height="18">斯洛文尼亚</td>
<td width="16%" align="center" height="18">SI</td>
<td width="16%" align="center" height="18">386</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Solomon Is</td>
<td width="21%" align="center" height="18">所罗门群岛</td>
<td width="16%" align="center" height="18">SB</td>
<td width="16%" align="center" height="18">677</td>
<td width="17%" align="center" height="18">+3</td>
</tr>
<tr>
<td width="30%" height="18">Somali</td>
<td width="21%" align="center" height="18">索马里</td>
<td width="16%" align="center" height="18">SO</td>
<td width="16%" align="center" height="18">252</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">South Africa</td>
<td width="21%" align="center" height="18">南非</td>
<td width="16%" align="center" height="18">ZA</td>
<td width="16%" align="center" height="18">27</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Spain</td>
<td width="21%" align="center" height="18">西班牙</td>
<td width="16%" align="center" height="18">ES</td>
<td width="16%" align="center" height="18">34</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">Sri Lanka</font></td>
<td width="21%" align="center" height="18">斯里兰卡</td>
<td width="16%" align="center" height="18">LK</td>
<td width="16%" align="center" height="18">94</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Sudan</td>
<td width="21%" align="center" height="18">苏丹</td>
<td width="16%" align="center" height="18">SD</td>
<td width="16%" align="center" height="18">249</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Suriname</td>
<td width="21%" align="center" height="18">苏里南</td>
<td width="16%" align="center" height="18">SR</td>
<td width="16%" align="center" height="18">597</td>
<td width="17%" align="center" height="18">-11.3</td>
</tr>
<tr>
<td width="30%" height="18">Swaziland</td>
<td width="21%" align="center" height="18">斯威士兰</td>
<td width="16%" align="center" height="18">SZ</td>
<td width="16%" align="center" height="18">268</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Sweden</td>
<td width="21%" align="center" height="18">瑞典</td>
<td width="16%" align="center" height="18">SE</td>
<td width="16%" align="center" height="18">46</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Switzerland</td>
<td width="21%" align="center" height="18">瑞士</td>
<td width="16%" align="center" height="18">CH</td>
<td width="16%" align="center" height="18">41</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Syria</td>
<td width="21%" align="center" height="18">叙利亚</td>
<td width="16%" align="center" height="18">SY</td>
<td width="16%" align="center" height="18">963</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Taiwan</td>
<td width="21%" align="center" height="18">台湾省</td>
<td width="16%" align="center" height="18">TW</td>
<td width="16%" align="center" height="18">886</td>
<td width="17%" align="center" height="18">0</td>
</tr>
<tr>
<td width="30%" height="18">Tajikstan</td>
<td width="21%" align="center" height="18">塔吉克斯坦</td>
<td width="16%" align="center" height="18">TJ</td>
<td width="16%" align="center" height="18">992</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Tanzania</td>
<td width="21%" align="center" height="18">坦桑尼亚</td>
<td width="16%" align="center" height="18">TZ</td>
<td width="16%" align="center" height="18">255</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Thailand</td>
<td width="21%" align="center" height="18">泰国</td>
<td width="16%" align="center" height="18">TH</td>
<td width="16%" align="center" height="18">66</td>
<td width="17%" align="center" height="18">-1</td>
</tr>
<tr>
<td width="30%" height="18">Togo</td>
<td width="21%" align="center" height="18">多哥</td>
<td width="16%" align="center" height="18">TG</td>
<td width="16%" align="center" height="18">228</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18">Tonga</td>
<td width="21%" align="center" height="18">汤加</td>
<td width="16%" align="center" height="18">TO</td>
<td width="16%" align="center" height="18">676</td>
<td width="17%" align="center" height="18">+4</td>
</tr>
<tr>
<td width="30%" height="18">Trinidad and Tobago</td>
<td width="21%" align="center" height="18">特立尼达和多巴哥</td>
<td width="16%" align="center" height="18">TT</td>
<td width="16%" align="center" height="18">1868</td>
<td width="17%" align="center" height="18">-12</td>
</tr>
<tr>
<td width="30%" height="18">Tunisia</td>
<td width="21%" align="center" height="18">突尼斯</td>
<td width="16%" align="center" height="18">TN</td>
<td width="16%" align="center" height="18">216</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Turkey</td>
<td width="21%" align="center" height="18">土耳其</td>
<td width="16%" align="center" height="18">TR</td>
<td width="16%" align="center" height="18">90</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Turkmenistan </td>
<td width="21%" align="center" height="18">土库曼斯坦</td>
<td width="16%" align="center" height="18">TM</td>
<td width="16%" align="center" height="18">993</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Uganda</td>
<td width="21%" align="center" height="18">乌干达</td>
<td width="16%" align="center" height="18">UG</td>
<td width="16%" align="center" height="18">256</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Ukraine</td>
<td width="21%" align="center" height="18">乌克兰</td>
<td width="16%" align="center" height="18">UA</td>
<td width="16%" align="center" height="18">380</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">United Arab
Emirates</font></td>
<td width="21%" align="center" height="18">阿拉伯联合酋长国</td>
<td width="16%" align="center" height="18">AE</td>
<td width="16%" align="center" height="18">971</td>
<td width="17%" align="center" height="18">-4</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">United Kiongdom</font></td>
<td width="21%" align="center" height="18">英国</td>
<td width="16%" align="center" height="18">GB</td>
<td width="16%" align="center" height="18">44</td>
<td width="17%" align="center" height="18">-8</td>
</tr>
<tr>
<td width="30%" height="18"><font color="#000000">United States
of America</font></td>
<td width="21%" align="center" height="18">美国</td>
<td width="16%" align="center" height="18">US</td>
<td width="16%" align="center" height="18">1</td>
<td width="17%" align="center" height="18">-13</td>
</tr>
<tr>
<td width="30%" height="18">Uruguay</td>
<td width="21%" align="center" height="18">乌拉圭</td>
<td width="16%" align="center" height="18">UY</td>
<td width="16%" align="center" height="18">598</td>
<td width="17%" align="center" height="18">-10.3</td>
</tr>
<tr>
<td width="30%" height="18">Uzbekistan</td>
<td width="21%" align="center" height="18">乌兹别克斯坦</td>
<td width="16%" align="center" height="18">UZ</td>
<td width="16%" align="center" height="18">998</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Venezuela</td>
<td width="21%" align="center" height="18">委内瑞拉</td>
<td width="16%" align="center" height="18">VE</td>
<td width="16%" align="center" height="18">58</td>
<td width="17%" align="center" height="18">-12.3</td>
</tr>
<tr>
<td width="30%" height="18">Vietnam</td>
<td width="21%" align="center" height="18">越南</td>
<td width="16%" align="center" height="18">VN</td>
<td width="16%" align="center" height="18">84</td>
<td width="17%" align="center" height="18">-1</td>
</tr>
<tr>
<td width="30%" height="18">Yemen</td>
<td width="21%" align="center" height="18">也门</td>
<td width="16%" align="center" height="18">YE</td>
<td width="16%" align="center" height="18">967</td>
<td width="17%" align="center" height="18">-5</td>
</tr>
<tr>
<td width="30%" height="18">Yugoslavia</td>
<td width="21%" align="center" height="18">南斯拉夫</td>
<td width="16%" align="center" height="18">YU</td>
<td width="16%" align="center" height="18">381</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Zimbabwe</td>
<td width="21%" align="center" height="18">津巴布韦</td>
<td width="16%" align="center" height="18">ZW</td>
<td width="16%" align="center" height="18">263</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
<tr>
<td width="30%" height="18">Zaire</td>
<td width="21%" align="center" height="18">扎伊尔</td>
<td width="16%" align="center" height="18">ZR</td>
<td width="16%" align="center" height="18">243</td>
<td width="17%" align="center" height="18">-7</td>
</tr>
<tr>
<td width="30%" height="18">Zambia</td>
<td width="21%" align="center" height="18">赞比亚</td>
<td width="16%" align="center" height="18">ZM</td>
<td width="16%" align="center" height="18">260</td>
<td width="17%" align="center" height="18">-6</td>
</tr>
</tbody>
</table>
HTML;

$str = strip_tags($html);
$str_array = explode(PHP_EOL.PHP_EOL, $str);
$configPostgresql = array(
	'host' => '192.168.1.108',
	'port' => '5433',
	'username' => 'eotu',
	'password' => 'eotu',
	'dbname' => 'testeotu',
);

$connection = new Phalcon\Db\Adapter\Pdo\Postgresql($configPostgresql);
foreach($str_array as $str) {
	$arr = array_filter(explode(PHP_EOL, $str));
	if (count($arr) < 5) continue;
	var_dump($str);
	$success = $connection->insert(array('yutong', 'countries'), array($connection->getDefaultIdValue(), $arr[2], $arr[3], $arr[4]));
	var_dump($success);
}
