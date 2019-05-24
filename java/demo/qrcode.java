package com.realart.utils.qrcode;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import javax.imageio.ImageIO;

import com.realart.interfaces.QrCodeInterface;
import jp.sourceforge.qrcode.QRCodeDecoder;
import jp.sourceforge.qrcode.exception.DecodingFailedException;

import com.swetake.util.Qrcode;
import org.apache.commons.lang.StringUtils;

public class TwoDimensionCode {
	
	/**
	 * 生成二维码(QRCode)图片
	 * @param content 存储内容
	 * @param imgPath 图片路径
	 */
	public void encoderQRCode(String content, String imgPath) {
		this.encoderQRCode(content, imgPath, "png", 7);
	}
	
	/**
	 * 生成二维码(QRCode)图片
	 * @param content 存储内容
	 * @param output 输出流
	 */
	public void encoderQRCode(String content, OutputStream output) {
		this.encoderQRCode(content, output, "png", 7);
	}

	/**
	 * 生成二维码(QRCode)图片
	 * @param content 存储内容
	 * @param imgPath 图片路径
	 * @param imgType 图片类型
	 */
	public void encoderQRCode(String content, String imgPath, String imgType) {
		this.encoderQRCode(content, imgPath, imgType, 7);
	}
	
	/**
	 * 生成二维码(QRCode)图片
	 * @param content 存储内容
	 * @param output 输出流
	 * @param imgType 图片类型
	 */
	public void encoderQRCode(String content, OutputStream output, String imgType) {
		this.encoderQRCode(content, output, imgType, 7);
	}

	/**
	 * 生成二维码(QRCode)图片
	 * @param content 存储内容
	 * @param imgPath 图片路径
	 * @param imgType 图片类型
	 * @param size 二维码尺寸
	 */
	public void encoderQRCode(String content, String imgPath, String imgType, int size) {
		try {
			BufferedImage bufImg = this.qRCodeCommon(content, imgType, size);
			
			File imgFile = new File(imgPath);
			// 生成二维码QRCode图片
			ImageIO.write(bufImg, imgType, imgFile);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * 生成二维码(QRCode)图片
	 * @param content 存储内容
	 * @param output 输出流
	 * @param imgType 图片类型
	 * @param size 二维码尺寸
	 */
	public void encoderQRCode(String content, OutputStream output, String imgType, int size) {
		try {
			BufferedImage bufImg = this.qRCodeCommon(content, imgType, size);
			// 生成二维码QRCode图片
			ImageIO.write(bufImg, imgType, output);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 生成二维码(QRCode)图片的公共方法
	 * @param content 存储内容
	 * @param imgType 图片类型
	 * @param size 二维码尺寸
	 * @return
	 */
	private BufferedImage qRCodeCommon(String content, String imgType, int size) {
		BufferedImage bufImg = null;
		try {
			Qrcode qrcodeHandler = new Qrcode();
			// 设置二维码排错率，可选L(7%)、M(15%)、Q(25%)、H(30%)，排错率越高可存储的信息越少，但对二维码清晰度的要求越小
			qrcodeHandler.setQrcodeErrorCorrect('M');
			qrcodeHandler.setQrcodeEncodeMode('B');
			// 设置设置二维码尺寸，取值范围1-40，值越大尺寸越大，可存储的信息越大
			qrcodeHandler.setQrcodeVersion(size);
			// 获得内容的字节数组，设置编码格式
			byte[] contentBytes = content.getBytes("utf-8");
			// 图片尺寸
			int imgSize = (67 + 12 * (size - 1))*10;
			bufImg = new BufferedImage(imgSize, imgSize, BufferedImage.TYPE_INT_RGB);
			Graphics2D gs = bufImg.createGraphics();
            //加上下面这句就能变的圆滑了
            gs.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
			// 设置背景颜色
			gs.setBackground(Color.WHITE);
			gs.clearRect(0, 0, imgSize, imgSize);

			// 设定图像颜色> BLACK
			gs.setColor(Color.BLACK);
			// 设置偏移量，不设置可能导致解析出错
			int pixoff = 20;
			// 输出内容> 二维码
			if (contentBytes.length > 0 && contentBytes.length < 800) {
				boolean[][] codeOut = qrcodeHandler.calQrcode(contentBytes);
                //随机颜色画圆形
//                for (int i = 0; i < codeOut.length; i++) {
//                    for (int j = 0; j < codeOut.length; j++) {
//                        if (codeOut[j][i]) {
//                            String[] colors = new String[]{"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};
//                            String colorStr = "";
//                            for(int k=0;k<6;k++){
//                                colorStr += colors[(int)(Math.random() * 10)];
//                            }
//                            Color color = new Color(Integer.parseInt(colorStr,16));
//                            gs.setColor(color);
//                            gs.setColor(Color.BLACK);
//                            gs.drawRect(j * 30 + pixoff, i * 30 + pixoff, 30, 30);
//                            gs.setColor(color);
//                            gs.fillOval(j * 30 + pixoff, i * 30 + pixoff, 30, 30);
//                            gs.setColor(Color.BLACK);
//                            gs.drawLine(j * 30 + pixoff, i * 30 + pixoff, j * 30 + pixoff + 30, i * 30 + pixoff + 30);
//                            gs.drawLine(j * 30 + pixoff + 30, i * 30 + pixoff, j * 30 + pixoff, i * 30 + pixoff + 30);

//                            int x0 = j * 30 + pixoff;
//                            int y0 = i * 30 + pixoff;
//                            int w = 30;
//                            int[] xs = {x0+w/2,x0,x0+w,x0,x0+w};
//                            int[] ys = {y0,y0+w,y0+w/2,y0+w/2,y0+w};
//                            gs.fillPolygon(xs, ys, 5);
//                            gs.fillOval(x0+w/4,y0+w/4+2,w/2,w/2);
//                            gs.drawRect(x0,y0,w,w);
//                        }
//                    }
//                }
                //画圆形
				for (int i = 0; i < codeOut.length; i++) {
					for (int j = 0; j < codeOut.length; j++) {
						if (codeOut[j][i]) {
							gs.fillOval(j * 30 + pixoff, i * 30 + pixoff, 30, 30);
						}
					}
				}
                //画好圆形 画直线连接
                for (int i = 0; i < codeOut.length; i++) {
                    for (int j = 0; j < codeOut.length; j++) {
                        if(i <= codeOut.length-2){
                            if (codeOut[j][i] && codeOut[j][i+1]) {
                                gs.fillRect(j * 30 + pixoff, i * 30 + pixoff + 15, 30, 30);
                            }
                        }
                        if(j <= codeOut.length-2){
                            if (codeOut[j][i] && codeOut[j+1][i]) {
                                gs.fillRect(j * 30 + pixoff + 15, i * 30 + pixoff, 30, 30);
                            }
                        }
                    }
                }
                //画好圆形 画好直线连接 画外面圆角
                for (int i = 0; i < codeOut.length; i++) {
                    for (int j = 0; j < codeOut.length; j++) {
                        //不是空白跳过
                        if(codeOut[j][i]){
                            continue;
                        }
                        if(j>=1 && i>=1){
                            //左上角
                            if(codeOut[j-1][i-1] && codeOut[j-1][i] && codeOut[j][i-1]){
                                gs.setColor(Color.BLACK);
                                gs.fillRect(j * 30 + pixoff, i * 30 + pixoff, 30/2, 30/2);
                                gs.setColor(Color.WHITE);
                                gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 90, 180);
                                gs.setColor(Color.BLACK);
                            }
                        }
                        if(j>=1 && i<=codeOut.length-2){
                            //右上角
                            if(codeOut[j-1][i+1] && codeOut[j-1][i] && codeOut[j][i+1]){
                                gs.setColor(Color.BLACK);
                                gs.fillRect(j * 30 + pixoff, i * 30 + pixoff + 30/2, 30/2, 30/2);
                                gs.setColor(Color.WHITE);
                                gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 180, 270);
                                gs.setColor(Color.BLACK);
                            }
                        }
                        if(j<=codeOut.length-2 && i>=1){
                            //左下角
                            if(codeOut[j+1][i-1] && codeOut[j+1][i] && codeOut[j][i-1]){
                                gs.setColor(Color.BLACK);
                                gs.fillRect(j * 30 + pixoff + 30/2, i * 30 + pixoff, 30/2, 30/2);
                                gs.setColor(Color.WHITE);
                                gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 180, 270);
                                gs.setColor(Color.BLACK);
                            }
                        }
                        if(j<=codeOut.length-2 && i<=codeOut.length-2){
                            //右下角
                            if(codeOut[j+1][i+1] && codeOut[j+1][i] && codeOut[j][i+1]){
                                gs.setColor(Color.BLACK);
                                gs.fillRect(j * 30 + pixoff + 30/2, i * 30 + pixoff + 30/2, 30/2, 30/2);
                                gs.setColor(Color.WHITE);
                                gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 270, 360);
                                gs.setColor(Color.BLACK);
                            }
                        }
                    }
                }
			} else {
				throw new Exception("QRCode content bytes length = " + contentBytes.length + " not in [0, 800].");
			}

            gs.setColor(Color.GREEN);
            gs.fillRoundRect((int)(imgSize*1.0/140*54), (int)(imgSize*1.0/140*54), (int)(imgSize*1.0/140*32), (int)(imgSize*1.0/140*32), 45, 45);

            Image img = ImageIO.read(new File("C:\\Users\\sky\\Desktop\\1.png"));//实例化一个Image对象。
            gs.drawImage(img, (int)(imgSize*1.0/140*55), (int)(imgSize*1.0/140*55), (int)(imgSize*1.0/140*30), (int)(imgSize*1.0/140*30), null);

			gs.dispose();
			bufImg.flush();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return bufImg;
	}
	
	/**
	 * 解析二维码（QRCode）
	 * @param imgPath 图片路径
	 * @return
	 */
	public String decoderQRCode(String imgPath) {
		// QRCode 二维码图片的文件
		File imageFile = new File(imgPath);
		BufferedImage bufImg = null;
		String content = null;
		try {
			bufImg = ImageIO.read(imageFile);
			QRCodeDecoder decoder = new QRCodeDecoder();
			content = new String(decoder.decode(new TwoDimensionCodeImage(bufImg)), "utf-8"); 
		} catch (IOException e) {
			System.out.println("Error: " + e.getMessage());
			e.printStackTrace();
		} catch (DecodingFailedException dfe) {
			System.out.println("Error: " + dfe.getMessage());
			dfe.printStackTrace();
		}
		return content;
	}
	
	/**
	 * 解析二维码（QRCode）
	 * @param input 输入流
	 * @return
	 */
	public String decoderQRCode(InputStream input) {
		BufferedImage bufImg = null;
		String content = null;
		try {
			bufImg = ImageIO.read(input);
			QRCodeDecoder decoder = new QRCodeDecoder();
			content = new String(decoder.decode(new TwoDimensionCodeImage(bufImg)), "utf-8"); 
		} catch (IOException e) {
			System.out.println("Error: " + e.getMessage());
			e.printStackTrace();
		} catch (DecodingFailedException dfe) {
			System.out.println("Error: " + dfe.getMessage());
			dfe.printStackTrace();
		}
		return content;
	}

    /**
     * 生成二维码(QRCode)图片的定制方法
     * @param content 存储内容
     * @param antiError 二维码排错率 可选L(7%)、M(15%)、Q(25%)、H(30%)，排错率越高可存储的信息越少，但对二维码清晰度的要求越小
     * @param size 二维码尺寸 取值范围1-40，值越大尺寸越大，可存储的信息越大
     * @param bgColor 背景颜色
     * @param frontColor 前景颜色
     * @param type 形态 1 液态 2 直角 3 圆形
     * @param qrLogoFile logo图片 null则不带logo
     * @param logoBorderType logo边缘 1 无边框 2 直角 3 圆角
     * @param logoBorderColor logo边缘颜色
     * @param imgRoute 保存图片路径
     * @return
     */
    public void customQrCode(String content, char antiError, int size, Color bgColor, Color frontColor,
                             String type, File qrLogoFile, String logoBorderType, Color logoBorderColor,
                             String imgRoute) throws Exception {
        BufferedImage bufImg = null;
        try {
            Qrcode qrcodeHandler = new Qrcode();
            // 设置二维码排错率，可选L(7%)、M(15%)、Q(25%)、H(30%)，排错率越高可存储的信息越少，但对二维码清晰度的要求越小
            qrcodeHandler.setQrcodeErrorCorrect(antiError);
            qrcodeHandler.setQrcodeEncodeMode('B');
            // 设置设置二维码尺寸，取值范围1-40，值越大尺寸越大，可存储的信息越大
            qrcodeHandler.setQrcodeVersion(size);
            // 获得内容的字节数组，设置编码格式
            byte[] contentBytes = content.getBytes("utf-8");
            // 图片尺寸
            int imgSize = (67 + 12 * (size - 1))*10;
            bufImg = new BufferedImage(imgSize, imgSize, BufferedImage.TYPE_INT_RGB);
            Graphics2D gs = bufImg.createGraphics();
            // 设置背景颜色
            gs.setBackground(bgColor);
            gs.clearRect(0, 0, imgSize, imgSize);

            // 设定图像颜色> BLACK
            gs.setColor(frontColor);
            // 设置偏移量，不设置可能导致解析出错
            int pixoff = 20;
            // 输出内容> 二维码
            if (contentBytes.length > 0 && contentBytes.length < 800) {
                boolean[][] codeOut = qrcodeHandler.calQrcode(contentBytes);
                if(StringUtils.equals("1", type)){//1 液态
                    //画圆形
                    for (int i = 0; i < codeOut.length; i++) {
                        for (int j = 0; j < codeOut.length; j++) {
                            if (codeOut[j][i]) {
                                gs.fillOval(j * 30 + pixoff, i * 30 + pixoff, 30, 30);
                            }
                        }
                    }
                    //画好圆形 画直线连接
                    for (int i = 0; i < codeOut.length; i++) {
                        for (int j = 0; j < codeOut.length; j++) {
                            if(i <= codeOut.length-2){
                                if (codeOut[j][i] && codeOut[j][i+1]) {
                                    gs.fillRect(j * 30 + pixoff, i * 30 + pixoff + 15, 30, 30);
                                }
                            }
                            if(j <= codeOut.length-2){
                                if (codeOut[j][i] && codeOut[j+1][i]) {
                                    gs.fillRect(j * 30 + pixoff + 15, i * 30 + pixoff, 30, 30);
                                }
                            }
                        }
                    }
                    //画好圆形 画好直线连接 画外面圆角
                    for (int i = 0; i < codeOut.length; i++) {
                        for (int j = 0; j < codeOut.length; j++) {
                            //不是空白跳过
                            if(codeOut[j][i]){
                                continue;
                            }
                            if(j>=1 && i>=1){
                                //左上角
                                if(codeOut[j-1][i-1] && codeOut[j-1][i] && codeOut[j][i-1]){
                                    gs.setColor(frontColor);
                                    gs.fillRect(j * 30 + pixoff, i * 30 + pixoff, 30/2, 30/2);
                                    gs.setColor(bgColor);
                                    gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 90, 180);
                                    gs.setColor(frontColor);
                                }
                            }
                            if(j>=1 && i<=codeOut.length-2){
                                //右上角
                                if(codeOut[j-1][i+1] && codeOut[j-1][i] && codeOut[j][i+1]){
                                    gs.setColor(frontColor);
                                    gs.fillRect(j * 30 + pixoff, i * 30 + pixoff + 30/2, 30/2, 30/2);
                                    gs.setColor(bgColor);
                                    gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 180, 270);
                                    gs.setColor(frontColor);
                                }
                            }
                            if(j<=codeOut.length-2 && i>=1){
                                //左下角
                                if(codeOut[j+1][i-1] && codeOut[j+1][i] && codeOut[j][i-1]){
                                    gs.setColor(frontColor);
                                    gs.fillRect(j * 30 + pixoff + 30/2, i * 30 + pixoff, 30/2, 30/2);
                                    gs.setColor(bgColor);
                                    gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 180, 270);
                                    gs.setColor(frontColor);
                                }
                            }
                            if(j<=codeOut.length-2 && i<=codeOut.length-2){
                                //右下角
                                if(codeOut[j+1][i+1] && codeOut[j+1][i] && codeOut[j][i+1]){
                                    gs.setColor(frontColor);
                                    gs.fillRect(j * 30 + pixoff + 30/2, i * 30 + pixoff + 30/2, 30/2, 30/2);
                                    gs.setColor(bgColor);
                                    gs.fillArc(j * 30 + pixoff, i * 30 + pixoff, 30, 30, 270, 360);
                                    gs.setColor(frontColor);
                                }
                            }
                        }
                    }
                } else if(StringUtils.equals("2", type)){//2 直角
                    //画直角
                    for (int i = 0; i < codeOut.length; i++) {
                        for (int j = 0; j < codeOut.length; j++) {
                            if (codeOut[j][i]) {
                                gs.fillRect(j * 30 + pixoff, i * 30 + pixoff, 30, 30);
                            }
                        }
                    }
                } else if(StringUtils.equals("3", type)){//3 圆形
                    //画圆形
                    for (int i = 0; i < codeOut.length; i++) {
                        for (int j = 0; j < codeOut.length; j++) {
                            if (codeOut[j][i]) {
                                gs.fillOval(j * 30 + pixoff, i * 30 + pixoff, 30, 30);
                            }
                        }
                    }
                }
            } else {
                throw new Exception("QRCode content bytes length = " + contentBytes.length + " not in [0, 800].");
            }

            //logo非空
            if(null != qrLogoFile){
                /**
                 * logo边缘 1 无边框 2 直角 3 圆角
                 */
                if(StringUtils.equals("1", logoBorderType)){//1 无边框
                    //do nothing
                } else if(StringUtils.equals("2", logoBorderType)){//2 直角
                    gs.setColor(logoBorderColor);
                    gs.fillRoundRect((int)(imgSize*1.0/140*54), (int)(imgSize*1.0/140*54), (int)(imgSize*1.0/140*32), (int)(imgSize*1.0/140*32), 0, 0);
                } else if(StringUtils.equals("3", logoBorderType)){//3 圆角
                    gs.setColor(logoBorderColor);
                    gs.fillRoundRect((int)(imgSize*1.0/140*54), (int)(imgSize*1.0/140*54), (int)(imgSize*1.0/140*32), (int)(imgSize*1.0/140*32), 45, 45);
                }

                /**
                 * logo图片 null则不带logo
                 */
                Image img = ImageIO.read(qrLogoFile);//实例化一个Image对象。
                gs.drawImage(img, (int)(imgSize*1.0/140*55), (int)(imgSize*1.0/140*55), (int)(imgSize*1.0/140*30), (int)(imgSize*1.0/140*30), null);
            }

            gs.dispose();
            bufImg.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }

        File imgFile = new File(imgRoute);
        // 生成二维码QRCode图片
        ImageIO.write(bufImg, "png", imgFile);
    }

    /**
     * main函数
     * @param args
     */
	public static void main(String[] args) {
		String imgPath = "";
		String content = QrCodeInterface.QR_CODE_URL_PREFIX + "12asdfbed";

		TwoDimensionCode handler = new TwoDimensionCode();
		handler.encoderQRCode(content, imgPath, "png", 3);
		System.out.println("========encoder success");

		//解析二维码
		String decoderContent = handler.decoderQRCode(imgPath);
		System.out.println("解析结果如下：");
		System.out.println(decoderContent);
		System.out.println("========decoder success!!!");
	}
}