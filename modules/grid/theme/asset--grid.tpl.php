<div class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <div class="content"<?php print $content_attributes; ?>>
    <?php
      hide($content['field_asset_ref_image']);
      print render($content);
    ?>
  </div>
</div>
