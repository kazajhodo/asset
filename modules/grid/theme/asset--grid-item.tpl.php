<div class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <div class="content"<?php print $content_attributes; ?>>
    <?php if(!empty($content['field_asset_textfield'])): ?>
      <h2>
        <?php
          if(!empty($content['field_asset_link'])){
            $content['field_asset_link'][0]['#element']['title'] = $content['field_asset_textfield'][0]['#markup'];
            hide($content['field_asset_textfield']);
            print render($content['field_asset_link']);
          }
          else{
            print render($content['field_asset_textfield']);
          }
        ?>
      </h2>
    <?php endif; ?>
    <?php
      print render($content);
    ?>
  </div>
</div>
